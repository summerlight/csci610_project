#ifndef __ALLOCATOR_HPP__
#define __ALLOCATOR_HPP__

#include <utility>
#include <unordered_set>
#include <vector>

#include "environment.hpp"
#include "finally.hpp"

template <typename T>
class allocator {
public:
    allocator()
    {
        get_exit_hooks().push_back([this] {
            return finalize();
        });
    }

    template<typename... U>
    T* alloc(U&&... value)
    {
        if (reserved_.empty()) {
            all_.emplace_back(new T(std::forward<U>(value)...));
            reserved_.push_back(all_.back().get());
        }
        auto* mem = reserved_.back();
        reserved_.pop_back();
        in_use_.insert(mem);
        if (EnableLogging) {
            fmt::print("[{}] alloc()-> {}\n", environment::get().get_thread_id(), (intptr_t)mem);
        }
        return mem;
    }
    
    void free(T* mem)
    {
        if (mem == nullptr) {
            return;
        }
        if (EnableLogging) {
            fmt::print("[{}] free({})\n", environment::get().get_thread_id(), (intptr_t)mem);
        }
        auto it = in_use_.find(mem);
        if (it == in_use_.end()) {
            throw_exception("double free");
        }
        in_use_.erase(it);
        reserved_.push_back(mem);
    }

    bool is_alive(T* mem) {
        return in_use_.find(mem) != in_use_.end();
    }

    bool finalize()
    {
        auto _ = finally([this] {
            all_.clear();
            reserved_.clear();
            in_use_.clear();
        });

        if (!in_use_.empty()) {
            return false;
            //throw_exception("memory leak");
        }
        return true;
    }

    std::vector<std::unique_ptr<T> > all_;
    std::vector<T*> reserved_;
    std::unordered_set<T*> in_use_;
    static allocator<T> singleton_;
};

template <typename T>
allocator<T> allocator<T>::singleton_;

template <typename T, typename... U>
static T* alloc(U&&... args) {
    return allocator<T>::singleton_.alloc(std::forward<U>(args)...);
}

template <typename T>
static void free(T* mem) {
    allocator<T>::singleton_.free(mem);
}

template <typename T>
static bool is_alive(T* mem) {
    return allocator<T>::singleton_.is_alive(mem);
}

#endif // __ALLOCATOR_HPP__