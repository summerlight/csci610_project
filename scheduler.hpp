#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <vector>

class scheduler {
public:
    virtual void ready(int thread_id) = 0;
    virtual int next() = 0;
};

class random_scheduler : public scheduler {
    std::vector<int> active_;

public:
    void ready(int thread_id) override;
    int next() override;
};

/*

class random_scheduler : public boost::fibers::algo::algorithm {
public:
    random_scheduler() = default;

    random_scheduler(random_scheduler const&) = delete;
    random_scheduler& operator=(random_scheduler const&) = delete;

    std::vector<boost::fibers::context*> ctx_;

    void awakened(boost::fibers::context* ctx) noexcept override
    {
        fmt::print("random_scheduler::awakened(context), ctx = {};\n", (intptr_t)ctx);
        ctx_.emplace_back(ctx);
        ranges::shuffle(ctx_);
    }

    boost::fibers::context* pick_next() noexcept override
    {
        if (ctx_.empty()) {
            return nullptr;
        }
        auto* ret = ctx_.back();
        ctx_.pop_back();
        fmt::print("random_scheduler::pick_next(), ret = {}, size = {};\n", (intptr_t)ret, ctx_.size());
        return ret;
    }

    bool has_ready_fibers() const noexcept override
    {
        fmt::print("random_scheduler::has_ready_fibers();\n");
        return !ctx_.empty();
    }

    void suspend_until(std::chrono::steady_clock::time_point const& tp) noexcept override
    {
        // Currently we don't want to implement timing related things
        fmt::print("random_scheduler::suspend_until();\n");
        assert(false);
    }

    void notify() noexcept override
    {
        // Currently we don't want to implement timing related things
        fmt::print("random_scheduler::notify();\n");
        assert(false);
    }
};

class prefix_scheduler : public boost::fibers::algo::algorithm {
public:
    prefix_scheduler() = default;

    prefix_scheduler(prefix_scheduler const&) = delete;
    prefix_scheduler& operator=(prefix_scheduler const&) = delete;

    boost::fibers::context* ctxs_[MaxThreadCnt] = {
        nullptr,
    };
    size_t ctx_cnt_{ 0 };

    std::vector<int> history_;
    size_t cur_{ 0 };
    std::mt19937 rnd_{ 0 };

    void awakened(boost::fibers::context* ctx) noexcept override
    {
        fmt::print("scheduler::awakened(context), thread_id = {};\n", std::this_thread::get_id());
        size_t id = environment::get().get_thread_id(ctx->get_id());
        assert(id < MaxThreadCnt && ctxs_[id] == nullptr);
        ctxs_[id] = ctx;
        ctx_cnt_++;
    }

    boost::fibers::context* pick_next() noexcept override
    {
        fmt::print("scheduler::pick_next(), thread_id = {};\n", std::this_thread::get_id());
        if (cur_ >= history_.size()) {
            // fill it
        }

        if (cur_ < history_.size()) {
            size_t id = history_[cur_++];
            auto* ctx = ctxs_[id];
            if (ctx != nullptr) {
                ctxs_[id] = nullptr;
                ctx_cnt_--;
                return ctx;
            } else {
                return nullptr;
            }
        }
        else {
            std::uniform_int_distribution<size_t> pick(0, MaxThreadCnt);
            auto a = pick(rnd_);
            return nullptr;
        }
    }

    bool has_ready_fibers() const noexcept override
    {
        fmt::print("scheduler::has_ready_fibers(), thread_id = {};\n", std::this_thread::get_id());
        return ctx_cnt_ != 0;
    }

    void suspend_until(std::chrono::steady_clock::time_point const& tp) noexcept override
    {
        // Currently we don't want to implement timing related things
        assert(false);
    }

    void notify() noexcept override
    {
        // Currently we don't want to implement timing related things
        assert(false);
    }
};
*/
#endif // __SCHEDULER_HPP__