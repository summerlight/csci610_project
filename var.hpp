#ifndef __VAR_HPP__
#define __VAR_HPP__

#include <cassert>
#include <exception>
#include <range/v3/view/iota.hpp>
#include "thread.hpp"
/*
template <typename T, int NumThread>
class atomic {

};
*/

template <typename T, int NumThread>
class atomic {
    T val_;
};

template <typename T, int NumThread>
class var {
public:
    T val_;
    uint32_t load_timestamp_[NumThread] = {
        0,
    };
    uint32_t store_timestamp_[NumThread] = {
        0,
    };

    var(T val, thread_context<NumThread>& th)
        : val_(val)
    {
        th.local_acq_rel_clock_ += 1;
        store_timestamp_[th.index_] = th.local_acq_rel_clock_;
    }

    ~var()
    {
    }

    T load(thread_context<NumThread>& th)
    {
        if (!check_load(th)) {
            throw std::runtime_error("data race");
        }
        return val_;
    }

    void store(thread_context<NumThread>& th, const T& v)
    {
        if (!check_store(th)) {
            throw std::runtime_error("data race");
        }
        val_ = v;
    }

    bool check_store(thread_context<NumThread>& th)
    {
        for (auto i : ranges::view::ints(0, NumThread)) {
            if (th.acq_rel_clock_[i] < store_timestamp_[i]) {
                return false;
            }
            if (th.acq_rel_clock_[i] < load_timestamp_[i]) {
                return false;
            }
        }

        th.local_acq_rel_clock_ += 1;
        store_timestamp_[th.index_] = th.local_acq_rel_clock_;
        return true;
    }

    bool check_load(thread_context<NumThread>& th)
    {
        for (auto i : ranges::view::ints(0, NumThread)) {
            if (th.acq_rel_clock_[i] < store_timestamp_[i]) {
                return false;
            }
        }

        th.local_acq_rel_clock_ += 1;
        load_timestamp_[th.index_] = th.local_acq_rel_clock_;
        return true;
    }
};

#endif // __VAR_HPP__