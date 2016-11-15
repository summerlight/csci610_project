#ifndef __THREAD_HPP__
#define __THREAD_HPP__

#include <cstddef>
#include <chrono>
#include <utility>

#include <boost/fiber/all.hpp>
#include <range/v3/view/iota.hpp>

template <int NumThread>
class thread_context {
public:
    thread_context(size_t index)
        : index_(index)
        , local_acq_rel_clock_(acq_rel_clock_[index])
    {
    }

    size_t index_;
    uint32_t& local_acq_rel_clock_;
    // TODO: These are essentially vector clocks, should be able to extract to standalone class
    uint32_t acq_rel_clock_[NumThread] = {
        0,
    };
    uint32_t acquire_clock_[NumThread] = {
        0,
    };
    uint32_t release_clock_[NumThread] = {
        0,
    };

    void fence_acquire()
    {
        for (auto i : ranges::view::ints(0, NumThread)) {
            if (acquire_clock_[i] > acq_rel_clock_[i]) {
                acq_rel_clock_[i] = acquire_clock_[i];
            }
        }
    }

    void fence_release()
    {
        for (auto i : ranges::view::ints(0, NumThread)) {
            release_clock_[i] = acq_rel_clock_[i];
        }
    }

    void fence_acq_rel()
    {
        fence_acquire();
        fence_release();
    }

    void fence_seq_cst(uint32_t seq_cst_order[NumThread])
    {
        fence_acquire();
        for (auto i : ranges::view::ints(0, NumThread)) {
            if (seq_cst_order[i] > acq_rel_clock_[i]) {
                acq_rel_clock_[i] = seq_cst_order[i];
            }
        }
        for (auto i : ranges::view::ints(0, NumThread)) {
            seq_cst_order[i] = acq_rel_clock_[i];
        }
        fence_release();
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