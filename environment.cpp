#include "environment.hpp"

#include <utility>
#include <exception>
#include "finally.hpp"
#include "scheduler.hpp"

std::unique_ptr<environment> singleton_;

void thread_context::set_id(size_t id)
{
    index_ = id;
    for (size_t i = 0; i < MaxFuncCnt; i++) {
        coverage_[i].thread_id = id;
        coverage_[i].func_no = i;
        coverage_[i].reset();
    }
}

std::vector<std::function<void()> >& get_exit_hooks()
{
    static std::vector<std::function<void()> > hooks;
    return hooks;
}

void throw_exception(const std::string& str)
{
    environment::log("Exception: {}\n", str);
    environment::get().error_ = str;
    //throw std::runtime_error(str);
}

environment::environment()
{
    for (size_t i = 0; i < MaxThreadCnt; i++) {
        threads_[i].set_id(i);
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
    environment::log("fence_seq_cst {}\n", new_clock);
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
            environment::log("unhandled exception : {}\n", e.what());
            error_ = e.what();
        }
        return switch_();
    };
}

void environment::run_impl()
{
    auto _ = finally([&] {
        finalize();
    });

    for (int i = 0; i < MaxThreadCnt; i++) {
        auto& ctx = context_[i];
        if (ctx) {
            scheduler_->ready(i);
        }
    }

    current_idx_ = scheduler_->next();
    while (current_idx_ != -1 && !is_finished()) {
        auto& picked = context_[current_idx_];
        if (picked) {
            picked = picked();
            history_.emplace_back(current_idx_, current_coverage());
            scheduler_->ready(current_idx_);
        }
        current_idx_ = scheduler_->next();
    }
}

bool environment::is_finished()
{
    return std::none_of(std::begin(context_), std::end(context_), [](auto& ctx) { return bool(ctx); });
}

bool environment::run()
{
    run_impl();
    return !error_.empty();
}

void environment::finalize()
{
    for (auto&& fn : get_exit_hooks()) {
        fn();
    }
}

size_t environment::current_coverage()
{
    size_t path_cov = 0;
    for (int i = 0; i < MaxThreadCnt; i++) {
        for (int j = 0; j < MaxFuncCnt; j++) {
            path_cov += threads_[i].coverage_[j].path_.count();
        }
    }
    return path_cov;
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