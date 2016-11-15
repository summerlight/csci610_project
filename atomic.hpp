#ifndef __ATOMIC_HPP__
#define __ATOMIC_HPP__

// TODO: should implement the below codes
template <class T>
struct atomic;

template <>
struct atomic<Integral>;

//template<>
//struct atomic<bool>;

template <class T>
struct atomic<T*>;

using atomic_char = atomic<char>;
using atomic_schar = atomic<signed char>;
using atomic_uchar = atomic<unsigned char>;
using atomic_short = atomic<short>;
using atomic_ushort = atomic<unsigned short>;
using atomic_int = atomic<int>;
using atomic_uint = atomic<unsigned int>;
using atomic_long = atomic<long>;
using atomic_ulong = atomic<unsigned long>;
using atomic_llong = atomic<long long>;
using atomic_ullong = atomic<unsigned long long>;
using atomic_char16_t = atomic<char16_t>;
using atomic_char32_t = atomic<char32_t>;
using atomic_wchar_t = atomic<wchar_t>;
using atomic_int8_t = atomic<std::int8_t>;
using atomic_uint8_t = atomic<std::uint8_t>;
using atomic_int16_t = atomic<std::int16_t>;
using atomic_uint16_t = atomic<std::uint16_t>;
using atomic_int32_t = atomic<std::int32_t>;
using atomic_uint32_t = atomic<std::uint32_t>;
using atomic_int64_t = atomic<std::int64_t>;
using atomic_uint64_t = atomic<std::uint64_t>;
using atomic_int_least8_t = atomic<std::int_least8_t>;
using atomic_uint_least8_t = atomic<std::uint_least8_t>;
using atomic_int_least16_t = atomic<std::int_least16_t>;
using atomic_uint_least16_t = atomic<std::uint_least16_t>;
using atomic_int_least32_t = atomic<std::int_least32_t>;
using atomic_uint_least32_t = atomic<std::uint_least32_t>;
using atomic_int_least64_t = atomic<std::int_least64_t>;
using atomic_uint_least64_t = atomic<std::uint_least64_t>;
using atomic_int_fast8_t = atomic<std::int_fast8_t>;
using atomic_uint_fast8_t = atomic<std::uint_fast8_t>;
using atomic_int_fast16_t = atomic<std::int_fast16_t>;
using atomic_uint_fast16_t = atomic<std::uint_fast16_t>;
using atomic_int_fast32_t = atomic<std::int_fast32_t>;
using atomic_uint_fast32_t = atomic<std::uint_fast32_t>;
using atomic_int_fast64_t = atomic<std::int_fast64_t>;
using atomic_uint_fast64_t = atomic<std::uint_fast64_t>;
using atomic_intptr_t = atomic<std::intptr_t>;
using atomic_uintptr_t = atomic<std::uintptr_t>;
using atomic_size_t = atomic<std::size_t>;
using atomic_ptrdiff_t = atomic<std::ptrdiff_t>;
using atomic_intmax_t = atomic<std::intmax_t>;
using atomic_uintmax_t = atomic<std::uintmax_t>;

template <class T>
bool atomic_is_lock_free(const volatile atomic<T>* obj);
template <class T>
bool atomic_is_lock_free(const atomic<T>* obj);

template <class T>
void atomic_store(atomic<T>* obj, T desr);
template <class T>
void atomic_store(volatile atomic<T>* obj, T desr);

template <class T>
void atomic_store_explicit(atomic<T>* obj, T desr,
                           memory_order order);
template <class T>
void atomic_store_explicit(volatile atomic<T>* obj, T desr,
                           memory_order order);

template <class T>
T atomic_load(const atomic<T>* obj);
template <class T>
T atomic_load(const volatile atomic<T>* obj);

template <class T>
T atomic_load_explicit(const atomic<T>* obj,
                       std::memory_order order);
template <class T>
T atomic_load_explicit(const volatile atomic<T>* obj,
                       std::memory_order order);

template <class T>
T atomic_exchange(atomic<T>* obj, T desr);
template <class T>
T atomic_exchange(volatile atomic<T>* obj, T desr);

template <class T>
T atomic_exchange_explicit(atomic<T>* obj, T desr,
                           std::memory_order order);
template <class T>
T atomic_exchange_explicit(volatile atomic<T>* obj, T desr,
                           std::memory_order order);

template <class T>
bool atomic_compare_exchange_weak(atomic<T>* obj,
                                  T* expected, T desired);
template <class T>
bool atomic_compare_exchange_weak(volatile atomic<T>* obj,
                                  T* expected, T desired);

template <class T>
bool atomic_compare_exchange_strong(atomic<T>* obj,
                                    T* expected, T desired);
template <class T>
bool atomic_compare_exchange_strong(volatile atomic<T>* obj,
                                    T* expected, T desired);

template <class T>
bool atomic_compare_exchange_weak_explicit(atomic<T>* obj,
                                           T* expected, T desired,
                                           std::memory_order succ,
                                           std::memory_order fail);
template <class T>
bool atomic_compare_exchange_weak_explicit(volatile atomic<T>* obj,
                                           T* expected, T desired,
                                           std::memory_order succ,
                                           std::memory_order fail);

template <class T>
bool atomic_compare_exchange_strong_explicit(atomic<T>* obj,
                                             T* expected, T desired,
                                             std::memory_order succ,
                                             std::memory_order fail);
template <class T>
bool atomic_compare_exchange_strong_explicit(volatile atomic<T>* obj,
                                             T* expected, T desired,
                                             std::memory_order succ,
                                             std::memory_order fail);

template <class Integral>
Integral atomic_fetch_add(atomic<Integral>* obj, Integral arg);
template <class Integral>
Integral atomic_fetch_add(volatile atomic<Integral>* obj, Integral arg);

template <class Integral>
Integral atomic_fetch_add_explicit(atomic<Integral>* obj, Integral arg,
                                   std::memory_order order);
template <class Integral>
Integral atomic_fetch_add_explicit(volatile atomic<Integral>* obj, Integral arg,
                                   std::memory_order order);

template <class T>
T* atomic_fetch_add(atomic<T*>* obj, std::ptrdiff_t arg);
template <class T>
T* atomic_fetch_add(volatile atomic<T*>* obj, std::ptrdiff_t arg);

template <class T>
T* atomic_fetch_add_explicit(atomic<T*>* obj, std::ptrdiff_t arg,
                             std::memory_order order);
template <class T>
T* atomic_fetch_add_explicit(volatile atomic<T*>* obj, std::ptrdiff_t arg,
                             std::memory_order order);

template <class Integral>
Integral atomic_fetch_sub(atomic<Integral>* obj, Integral arg);
template <class Integral>
Integral atomic_fetch_sub(volatile atomic<Integral>* obj, Integral arg);

template <class Integral>
Integral atomic_fetch_sub_explicit(atomic<Integral>* obj, Integral arg,
                                   std::memory_order order);
template <class Integral>
Integral atomic_fetch_sub_explicit(volatile atomic<Integral>* obj, Integral arg,
                                   std::memory_order order);

template <class T>
T* atomic_fetch_sub(atomic<T*>* obj, std::ptrdiff_t arg);
template <class T>
T* atomic_fetch_sub(volatile atomic<T*>* obj, std::ptrdiff_t arg);

template <class T>
T* atomic_fetch_sub_explicit(atomic<T*>* obj, std::ptrdiff_t arg,
                             std::memory_order order);
template <class T>
T* atomic_fetch_sub_explicit(volatile atomic<T*>* obj, std::ptrdiff_t arg,
                             std::memory_order order);

template <class Integral>
Integral atomic_fetch_and(atomic<Integral>* obj, Integral arg);
template <class Integral>
Integral atomic_fetch_and(volatile atomic<Integral>* obj, Integral arg);

template <class Integral>
Integral atomic_fetch_and_explicit(atomic<Integral>* obj,
                                   Integral arg,
                                   std::memory_order order);
template <class Integral>
Integral atomic_fetch_and_explicit(volatile atomic<Integral>* obj,
                                   Integral arg,
                                   std::memory_order order);

template <class Integral>
Integral atomic_fetch_or(atomic<Integral>* obj, Integral arg);
template <class Integral>
Integral atomic_fetch_or(volatile atomic<Integral>* obj, Integral arg);

template <class Integral>
Integral atomic_fetch_or_explicit(atomic<Integral>* obj,
                                  Integral arg,
                                  std::memory_order order);
template <class Integral>
Integral atomic_fetch_or_explicit(volatile atomic<Integral>* obj,
                                  Integral arg,
                                  std::memory_order order);

template <class Integral>
Integral atomic_fetch_xor(atomic<Integral>* obj, Integral arg);
template <class Integral>
Integral atomic_fetch_xor(volatile atomic<Integral>* obj, Integral arg);

template <class Integral>
Integral atomic_fetch_xor_explicit(atomic<Integral>* obj, Integral arg,
                                   std::memory_order order);
template <class Integral>
Integral atomic_fetch_xor_explicit(volatile atomic<Integral>* obj, Integral arg,
                                   std::memory_order order);

class atomic_flag {
public:
    atomic_flag();
    atomic_flag(const atomic_flag&) = delete;
    atomic_flag& operator=(const atomic_flag&) = delete;
    atomic_flag& operator=(const atomic_flag&) volatile = delete;
    void clear(std::memory_order order = std::memory_order_seq_cst) volatile;
    void clear(std::memory_order order = std::memory_order_seq_cst);
    bool test_and_set(std::memory_order order = std::memory_order_seq_cst) volatile;
    bool test_and_set(std::memory_order order = std::memory_order_seq_cst);
};

template <class T>
void atomic_init(atomic<T>* obj, T desired);
template <class T>
void atomic_init(volatile atomic<T>* obj, T desired);

extern "C" void atomic_thread_fence(std::memory_order order);
extern "C" void atomic_signal_fence(std::memory_order order);

#endif // __ATOMIC_HPP__