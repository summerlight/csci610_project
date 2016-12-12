#ifndef __ENVIRONMENT_HPP__
#define __ENVIRONMENT_HPP__

#include <cstddef>
#include <bitset>
#include <functional>
#include <memory>
#include <string>
#include <unordered_set>
#include <boost/context/all.hpp>
#include "constant.hpp"
#include "instrument.hpp"
#include "scheduler.hpp"
#include "vector_clock.hpp"

class thread_context {
public:
    void set_id(size_t id);

    size_t index_ = 0;
    vector_clock<MaxThreadCnt> current_;
    //vector_clock<MaxThreadCnt> release_;
    code_coverage coverage_[MaxFuncCnt];
    std::unordered_set<intptr_t> data_cov_;
};

std::vector<std::function<void()> >& get_exit_hooks();
void throw_exception(const std::string& ex);

class history {
public:
    history(int thread, size_t coverage)
        : thread_(thread), coverage_(coverage)
    {
        
    }

    int thread_;
    size_t coverage_;
};

class environment {
    using context_t = boost::context::execution_context<void>;

public:
    bool enable_logging_ = false;
    thread_context threads_[MaxThreadCnt];
    int current_idx_ = -1;
    context_t switch_;
    context_t context_[MaxThreadCnt];
    std::unique_ptr<scheduler> scheduler_;
    std::string error_;
    std::vector<history> history_;

    environment();

    //void fence_acquire();
    //void fence_release();
    //void fence_acq_rel();
    vector_clock<MaxThreadCnt> fence_seq_cst();
    void yield();

    void register_thread(size_t idx, std::function<void()> fn);
    void run_impl();
    bool run();
    bool is_finished();
    void finalize();
    size_t current_coverage();

    int get_thread_id();
    thread_context& this_thread();
    static environment& get();
    static environment& reset();

    template<typename... Args>
    inline static void log(const char* fmt_str, Args&&... args)
    {
        if (get().enable_logging_) {
            fmt::print(fmt_str, std::forward<Args>(args)...);
        }
    }
};

#endif // __ENVIRONMENT_HPP__