#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include <cstddef>
#include <memory>
#include <map>
#include <boost/fiber/all.hpp>
#include <fmt/ostream.h>
#include "thread.hpp"

template <size_t NumThread>
class environment {
public:
    thread_context<NumThread> threads_[NumThread];
    std::map<boost::fibers::fiber::id, size_t> id_map{};

    environment() {
        for (size_t i = 0; i < NumThread; i++) {
            threads_[i].index_ = i;
        }
    }

    void fence_acquire()
    {
        boost::this_fiber::yield();
        auto& th = this_thread();
        for (const auto& others : threads_) {
            th.current_.merge(others.release_);
        }
        th.current_.increment(th.index_);
    }

    void fence_release()
    {
        boost::this_fiber::yield();
        auto& th = this_thread();
        th.release_ = th.current_;
        th.current_.increment(th.index_);
    }

    void fence_acq_rel()
    {
        boost::this_fiber::yield();
        fence_acquire();
        fence_release();
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

    void register_thread(size_t idx, std::function<void()> fn)
    {
        assert(idx < NumThread);
        auto& thread = threads_[idx];
        thread.fiber_ = boost::fibers::fiber{
            [this, &thread, idx, fn] {
                id_map[boost::this_fiber::get_id()] = idx;
                try {
                    fn();
                }
                catch (std::runtime_error&) {
                    thread.result_ = true;
                }
            }
        };
    }

    bool run()
    {
        bool result = false;
        for (auto& th : threads_) {
            th.fiber_.join();
            if (th.result_) {
                result = true;
            }
        }
        return result;
    }

    thread_context<NumThread>& this_thread()
    {
        const size_t id = id_map[boost::this_fiber::get_id()]; 
        return threads_[id];
    }

    static std::unique_ptr<environment<NumThread>> singleton_;
    static environment& get() {
        if (!singleton_) {
            singleton_.reset(new environment<NumThread>);
        }
        return *singleton_;
    }
    static environment& reset() {
        singleton_.reset(new environment<NumThread>);
        return *singleton_;
    }
};

template <size_t NumThread>
std::unique_ptr<environment<NumThread>> environment<NumThread>::singleton_;

#endif // __ENVIRONMENT_HPP__