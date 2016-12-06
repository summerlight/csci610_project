#ifndef __VAR_HPP__
#define __VAR_HPP__

#include <algorithm>
#include <cassert>
#include <exception>
#include <range/v3/view/iota.hpp>
#include "constant.hpp"
#include "environment.hpp"
#include "vector_clock.hpp"

template <typename T>
class var {
public:
    T value_;
    vector_clock<MaxThreadCnt> clock_;

    var(T value)
        : value_(value)
    {
    }

    ~var()
    {
    }

    void check()
    {
        auto& th = environment::get().this_thread();
        if (!clock_.happens_before(th.current_)) {
            if (EnableLogging) {
                fmt::print("[{}] data race({})({}, {})\n", th.index_, (intptr_t)this, th.current_, clock_);
            }
            throw_exception("data race");
        }
    }

    void log(const char* func)
    {
        if (EnableLogging) {
            fmt::print("[{}] var({})::{}({})\n", environment::get().get_thread_id(), (intptr_t)this, func, clock_);
        }
    }

    void store(const T& v)
    {
        environment::get().yield();
        check();
        auto& th = environment::get().this_thread();
        th.current_.increment(th.index_);
        clock_ = th.current_;
        log("store");
        value_ = v;
    }

    T load()
    {
        environment::get().yield();
        check();
        auto& th = environment::get().this_thread();
        log("load");
        return value_;
    }

    void atomic_store(const T& v)
    {
        clock_ = environment::get().fence_seq_cst();
        check();
        log("atomic_store");
        value_ = v;
    }

    T atomic_load()
    {
        clock_ = environment::get().fence_seq_cst();
        check();
        log("atomic_load");
        return value_;
    }

    template <typename Op>
    T atomic_rmw(Op op)
    {
        clock_ = environment::get().fence_seq_cst();
        check();
        log("atomic_rmw");
        auto old = value_;
        value_ = op(old);
        return old;
    }

    bool atomic_cas(T expected, T desired)
    {
        clock_ = environment::get().fence_seq_cst();
        check();
        log("atomic_cas");
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

template <typename T>
class var_ptr : public var<T*> {
public:
    using super = var<T*>;
    var_ptr(T* value)
        : super(value)
    {
    }

    T* load()
    {
        check_freed_memory();
        return super::load();
    }

    T* atomic_load()
    {
        check_freed_memory();
        return super::atomic_load();
    }

    template <typename Op>
    T* atomic_rmw(Op op)
    {
        check_freed_memory();
        return super::atomic_rmw(op);
    }

    bool atomic_cas(T* expected, T* desired)
    {
        check_freed_memory();
        return super::atomic_cas(expected, desired);
    }

    void check_freed_memory() {
        auto* ptr = super::raw_load();
        if (ptr && !is_alive(ptr)) {
            if (EnableLogging) {
                fmt::print("[{}] access to freed memory({}) {}\n", environment::get().get_thread_id(), (intptr_t)this, (intptr_t)super::raw_load());
            }
            throw_exception("access to freed memory");
        }
    }
};

#endif // __VAR_HPP__