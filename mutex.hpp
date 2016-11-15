#ifndef __MUTEX_HPP__
#define __MUTEX_HPP__

// TODO: implement these classes
// maybe we don't need timed mutexes
class mutex {
public:
    using native_handle_type = size_t;
    constexpr mutex();
    mutex(const mutex&) = delete;
    ~mutex();
    mutex& operator=(const mutex&) = delete;

    void lock();
    bool try_lock();
    void unlock();
};

class timed_mutex {
public:
    using native_handle_type = size_t;
    constexpr timed_mutex();
    timed_mutex(const timed_mutex&) = delete;
    ~timed_mutex();
    timed_mutex& operator=(const timed_mutex&) = delete;

    void lock();
    bool try_lock();
    void unlock();
    template <class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration);
    template <class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& timeout_time);
};

class recursive_mutex {
public:
    using native_handle_type = size_t;
    constexpr recursive_mutex();
    recursive_mutex(const recursive_mutex&) = delete;
    ~recursive_mutex();
    recursive_mutex& operator=(const recursive_mutex&) = delete;

    void lock();
    bool try_lock();
    void unlock();
};

class timed_recursive_mutex {
public:
    using native_handle_type = size_t;
    constexpr timed_recursive_mutex();
    timed_recursive_mutex(const timed_recursive_mutex&) = delete;
    ~timed_recursive_mutex();
    timed_recursive_mutex& operator=(const timed_recursive_mutex&) = delete;

    void lock();
    bool try_lock();
    void unlock();
    template <class Rep, class Period>
    bool try_lock_for(const std::chrono::duration<Rep, Period>& timeout_duration);
    template <class Clock, class Duration>
    bool try_lock_until(const std::chrono::time_point<Clock, Duration>& timeout_time);
};

class once_flag {
public:
    constexpr once_flag();
};

template <class Callable, class... Args>
void call_once(std::once_flag& flag, Callable&& f, Args&&... args);



#endif // __MUTEX_HPP__