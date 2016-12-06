#include "environment.hpp"

#include <exception>
#include "finally.hpp"
#include "scheduler.hpp"

std::unique_ptr<environment> singleton_;

std::vector<std::function<bool()> >& get_exit_hooks()
{
    static std::vector<std::function<bool()> > hooks;
    return hooks;
}

void throw_exception(const char* ex)
{
    throw std::runtime_error(ex);
}

environment::environment()
{
    for (size_t i = 0; i < MaxThreadCnt; i++) {
        threads_[i].index_ = i;
    }
    scheduler_.reset(new random_scheduler);
}
/*
void environment::fence_acquire()
{
    yield();
    auto& th = this_thread();
    for (const auto& others : threads_) {
        th.current_.merge(others.release_);
    }
    th.current_.increment(th.index_);
}

void environment::fence_release()
{
    yield();
    auto& th = this_thread();
    th.release_ = th.current_;
    th.current_.increment(th.index_);
}

void environment::fence_acq_rel()
{
    yield();
    fence_acquire();
    fence_release();
}
*/
vector_clock<MaxThreadCnt> environment::fence_seq_cst()
{
    yield();
    vector_clock<MaxThreadCnt> new_clock;
    for (const auto& th : threads_) {
        new_clock.merge(th.current_);
    }
    for (auto& th : threads_) {
        th.current_ = new_clock;
        th.current_.increment(th.index_);
    }
    if (EnableLogging) {
        fmt::print("fence_seq_cst {}\n", new_clock);
    }
    return new_clock;
}

void environment::yield()
{
    switch_ = switch_();
}

void environment::register_thread(size_t idx, std::function<void()> fn)
{
    assert(idx < MaxThreadCnt);
    context_[idx] = [this, idx, fn](context_t ctx) {
        switch_ = std::move(ctx);
        try {
            fn();
        }
        catch (std::runtime_error& e) {
            if (EnableLogging) {
                fmt::print("exception : {}\n", e.what());
            }
            error_ = e.what();
        }
        return switch_();
    };
}

bool environment::run()
{
    bool ret = false;
    auto _ = finally([&] {
        ret = finalize();
    });

    for (int i = 0; i < MaxThreadCnt; i++) {
        auto& ctx = context_[i];
        if (ctx) {
            scheduler_->ready(i);
        }
    }

    current_idx_ = scheduler_->next();
    while (current_idx_ != -1) {
        auto& picked = context_[current_idx_];
        if (picked) {
            picked = picked();
            if (!error_.empty()) {
                return true;
            }
            scheduler_->ready(current_idx_);
        }
        current_idx_ = scheduler_->next();
    }

    return ret;
}

bool environment::finalize()
{
    for (auto&& fn : get_exit_hooks()) {
        if (!fn()) {
            return true;
        }
    }
    return false;
}

int environment::get_thread_id()
{
    return current_idx_;
}

thread_context& environment::this_thread()
{
    return threads_[current_idx_];
}

environment& environment::get()
{
    if (!singleton_) {
        singleton_.reset(new environment);
    }
    return *singleton_;
}

environment& environment::reset()
{
    singleton_.reset(new environment);
    return *singleton_;
}