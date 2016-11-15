#ifndef __CONDITION_VARIABLE__
#define __CONDITION_VARIABLE__

#include "mutex.hpp"
#include <chrono>

// TODO: Do we need this? 

enum class cv_status {
    no_timeout = 0,
    timeout = 1
};

class condition_variable {
public:
    using native_handle_type = size_t;

    condition_variable();
    condition_variable(const condition_variable&) = delete;

    ~condition_variable();

    condition_variable& operator=(const condition_variable&) = delete;

    void notify_one();
    void notify_all();

    void wait(std::unique_lock<mutex>& lock);
    template <class Predicate>
    void wait(std::unique_lock<mutex>& lock, Predicate pred);

    template <class Rep, class Period>
    cv_status wait_for(std::unique_lock<mutex>& lock,
                       const std::chrono::duration<Rep, Period>& rel_time);

    template <class Rep, class Period, class Predicate>
    bool wait_for(std::unique_lock<std::mutex>& lock,
                  const std::chrono::duration<Rep, Period>& rel_time,
                  Predicate pred);

    template <class Clock, class Duration>
    cv_status wait_until(std::unique_lock<mutex>& lock,
                         const std::chrono::time_point<Clock, Duration>& timeout_time);
    template <class Clock, class Duration, class Predicate>
    bool wait_until(std::unique_lock<mutex>& lock,
                    const std::chrono::time_point<Clock, Duration>& timeout_time,
                    Predicate pred);

    native_handle_type native_handle();
};

class condition_variable_any {
public:
    using native_handle_type = size_t;

    condition_variable_any();
    condition_variable_any(const condition_variable_any&) = delete;

    ~condition_variable_any();

    condition_variable_any& operator=(const condition_variable_any&) = delete;

    void notify_one();
    void notify_all();

    template <class Lock>
    void wait(Lock& lock);
    template <class Lock, class Predicate>
    void wait(Lock& lock, Predicate pred);

    template <class Lock, class Rep, class Period>
    cv_status wait_for(Lock& lock,
                       const std::chrono::duration<Rep, Period>& rel_time);
    template <class Lock, class Rep, class Period, class Predicate>
    bool wait_for(Lock& lock,
                  const std::chrono::duration<Rep, Period>& rel_time,
                  Predicate pred);

    template <class Lock, class Clock, class Duration>
    cv_status wait_until(Lock& lock,
                         const std::chrono::time_point<Clock, Duration>& timeout_time);
    template <class Lock, class Clock, class Duration, class Predicate>
    bool wait_until(Lock& lock,
                    const std::chrono::time_point<Clock, Duration>& timeout_time,
                    Predicate pred);

    native_handle_type native_handle();
};

void notify_all_at_thread_exit(condition_variable& cond,
                               std::unique_lock<mutex> lk);

#endif // __CONDITION_VARIABLE__