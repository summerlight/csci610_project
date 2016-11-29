#ifndef __VAR_HPP__
#define __VAR_HPP__

#include <algorithm>
#include <cassert>
#include <exception>
#include <range/v3/view/iota.hpp>
#include "environment.hpp"
#include "thread.hpp"
#include "vector_clock.hpp"

template <typename T, size_t NumThread>
class atomic {
public:
    T value_;

    atomic(T value)
        : value_(value)
    {
    }

    ~atomic()
    {
    }

    T load()
    {
        environment<NumThread>::get().fence_seq_cst();
        //fmt::print("[{}] {} : load() -> {}\n", environment<NumThread>::get().this_thread().index_, (intptr_t)this, value_);
        return value_;
    }

    void store(const T& v)
    {
        environment<NumThread>::get().fence_seq_cst();
        //fmt::print("[{}] {} : store({})\n", environment<NumThread>::get().this_thread().index_, (intptr_t)this, v);
        value_ = v;
    }

    template <typename Op>
    T rmw(Op op)
    {
        environment<NumThread>::get().fence_seq_cst();
        auto old = value_;
        value_ = op(old);
        //fmt::print("[{}] {} : rmw({} -> {})\n", environment<NumThread>::get().this_thread().index_, (intptr_t)this, old, value_);
        return old;
    }

    bool cas(T expected, T desired)
    {
        environment<NumThread>::get().fence_seq_cst();
        if (value_ == expected) {
            value_ = desired;
            return true;
        }
        else {
            return false;
        }
    }

    T raw_load()
    {
        return value_;
    }
};

template <typename T, size_t NumThread>
class var {
public:
    T value_;
    vector_clock<NumThread> clock_;
    var(T value)
        : value_(value)
    {
    }

    var(thread_context<NumThread>& th, T value)
    {
        store(th, value);
    }

    ~var()
    {
    }

    void store(const T& v)
    {
        auto& th = environment<NumThread>::get().this_thread();
        fmt::print("store from thread {} / {} / {}\n", th.index_, clock_, th.current_);
        if (!clock_.happens_before(th.current_)) {
            throw std::runtime_error("data race");
        }
        clock_ = th.current_;
        th.current_.increment(th.index_);
        value_ = v;
    }

    T load()
    {
        auto& th = environment<NumThread>::get().this_thread();
        fmt::print("load from thread {} / {} / {}\n", th.index_, clock_, th.current_);
        if (!clock_.happens_before(th.current_)) {
            throw std::runtime_error("data race");
        }
        th.current_.increment(th.index_);
        return value_;
    }

    T raw_load()
    {
        return value_;
    }
};

#endif // __VAR_HPP__