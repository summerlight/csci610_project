#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <thread>
#include <vector>
#include <boost/fiber/all.hpp>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <range/v3/algorithm/shuffle.hpp>

class scheduler : public boost::fibers::algo::algorithm {
public:
    scheduler() = default;

    scheduler(scheduler const&) = delete;
    scheduler& operator=(scheduler const&) = delete;

    std::vector<boost::fibers::context*> ctx_;

    void awakened(boost::fibers::context* ctx) noexcept override
    {
		fmt::print("scheduler::awakened(context), thread_id = {};\n", std::this_thread::get_id());
        ctx_.emplace_back(ctx);
        ranges::shuffle(ctx_);
    }

    boost::fibers::context* pick_next() noexcept override
    {
		fmt::print("scheduler::pick_next(), thread_id = {};\n", std::this_thread::get_id());
        if (ctx_.empty()) {
            return nullptr;
        }
        auto* ret = ctx_.back();
        ctx_.pop_back();
        return ret;
    }

    bool has_ready_fibers() const noexcept override
    {
		fmt::print("scheduler::has_ready_fibers(), thread_id = {};\n", std::this_thread::get_id());
        return !ctx_.empty();
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

#endif // __SCHEDULER_HPP__