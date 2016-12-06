#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include <cstddef>
#include <bitset>
#include <functional>
#include <memory>
#include <string>
#include <boost/context/all.hpp>
#include "constant.hpp"
#include "instrument.hpp"
#include "scheduler.hpp"
#include "vector_clock.hpp"

class thread_context {
public:
    size_t index_ = 0;
    vector_clock<MaxThreadCnt> current_;
    //vector_clock<MaxThreadCnt> release_;
    code_coverage coverage_[MaxFuncCnt];
};

std::vector<std::function<bool()> >& get_exit_hooks();
void throw_exception(const char* ex);

class environment {
    using context_t = boost::context::execution_context<void>;
public:
    thread_context threads_[MaxThreadCnt];
    int current_idx_ = -1;
    context_t switch_;
    context_t context_[MaxThreadCnt];
    std::unique_ptr<scheduler> scheduler_;
    std::string error_;

    environment();

    //void fence_acquire();
    //void fence_release();
    //void fence_acq_rel();
    vector_clock<MaxThreadCnt> fence_seq_cst();
    void yield();

    void register_thread(size_t idx, std::function<void()> fn);
    bool run();
    bool finalize();

    int get_thread_id();
    thread_context& this_thread();
    static environment& get();
    static environment& reset();
};

#endif // __ENVIRONMENT_HPP__