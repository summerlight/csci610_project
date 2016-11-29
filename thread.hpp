#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <cstddef>
#include <chrono>
#include <utility>

#include <boost/fiber/all.hpp>
#include <range/v3/view/iota.hpp>

#include "vector_clock.hpp"

template <int NumThread>
class thread_context {
public:
    size_t index_ = 0;
    vector_clock<NumThread> current_;
    vector_clock<NumThread> release_;
    bool result_ = false;
    boost::fibers::fiber fiber_;
};

template <size_t NumThread>
class execution_context {
public:
    thread_context<NumThread> threads_[NumThread];

    execution_context()
    {
        for (size_t i = 0; i < NumThread; i++) {
            threads_[i].index_ = i;
        }
    }

    void fence_acquire(size_t idx)
    {
        auto& th = threads_[idx];
        for (const auto& others : threads_) {
            th.current_.merge(others.release_);
        }
        th.current_.increment(idx);
    }

    void fence_release(size_t idx)
    {
        auto& th = threads_[idx];
        th.release_ = th.current_;
        th.current_.increment(idx);
    }

    void fence_acq_rel(size_t idx)
    {
        fence_acquire(idx);
        fence_release(idx);
    }

    void fence_seq_cst()
    {
        boost::this_fiber::yield();
        vector_clock<NumThread> new_clock;
        for (const auto& th : threads_) {
            new_clock.merge(th.current_);
        }
        for (auto& th : threads_) {
            th.current_ = new_clock;
            th.current_.increment(th.index_);
        }
    }
};

// TODO: implement the below interface
namespace this_thread {
size_t get_id()
{
    return 0;
}
} // namespace this_thread

class thread {
public:
    using id = size_t;
    using native_handle_type = id;

    thread() {}
    thread(thread&& other)
        : id_(other.id_)
        , ctx_(std::move(other.ctx_))
    {
    }

    template <typename Function, class... Args>
    explicit thread(Function&& f, Args&&... args)
        : id_(1)
        , ctx_(f, std::forward<Args>(args)...)
    {
    }

    thread(const thread&) = delete;

    ~thread()
    {
        if (joinable()) {
            // std::terminate();
        }
    }

    thread& operator=(thread&& other)
    {
        id_ = other.id_;
        ctx_ = std::move(other.ctx_);
        return *this;
    }

    bool joinable() const
    {
        return get_id() == 0; // TODO: implement
    }

    id get_id() const
    {
        return id_;
    }

    native_handle_type native_handle()
    {
        return id_;
    }

    static unsigned hardware_concurrency()
    {
        return 1;
    }

    void join()
    {
        if (get_id() == ::this_thread::get_id()) {
            // throw std::resource_deadlock_would_occur;
        }
        // if (invalid_thread) { throw std::no_such_thread; }
        if (!joinable()) {
            // throw std::invalid_argument;
        }

        // TODO: implement
    }

    void detach()
    {
        if (!joinable()) {
            // throw std::system_error
        }
        // TODO: implement
    }

    void swap(thread& other)
    {
        std::swap(id_, other.id_);
        std::swap(ctx_, other.ctx_);
    }

    void __yield__()
    {
        ctx_ = ctx_();
    }

private:
    id id_ = 0; // TODO: allocate it
    boost::context::execution_context<void> ctx_;
};

void yield()
{
}

template <class Rep, class Period>
void sleep_for(const std::chrono::duration<Rep, Period>& sleep_duration)
{
}

template <class Clock, class Duration>
void sleep_until(const std::chrono::time_point<Clock, Duration>& sleep_time)
{
}

namespace std {
void swap(::thread& lhs, ::thread& rhs)
{
    lhs.swap(rhs);
}
} // namespace std

#endif // __THREAD_HPP__