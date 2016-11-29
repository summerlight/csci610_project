#ifndef __VECTOR_CLOCK_HPP__
#define __VECTOR_CLOCK_HPP__

#include <fmt/format.h>

template <size_t NumThread>
class vector_clock {
public:
    uint32_t clocks_[NumThread] = {
        0,
    };

    bool happens_before(const vector_clock<NumThread>& other) const
    {
        for (int i = 0; i < NumThread; i++) {
            if (clocks_[i] > other.clocks_[i]) {
                return false;
            }
        }
        return true;
    }

    bool is_concurrent(const vector_clock<NumThread>& other) const
    {
        return !happens_before(other) && !other.happens_before(*this);
    }

    void merge(const vector_clock<NumThread>& other)
    {
        for (int i = 0; i < NumThread; i++) {
            if (clocks_[i] < other.clocks_[i]) {
                clocks_[i] = other.clocks_[i];
            }
        }
    }

    void increment(size_t idx)
    {
        clocks_[idx]++;
    }

    void print()
    {
        fmt::print("{}\n", clocks_);
    }
};

template <typename Char, size_t NumThread>
void format_arg(fmt::BasicFormatter<Char>& f, const Char*, const vector_clock<NumThread>& clock)
{
    for (size_t i = 0; i < NumThread; i++) {
        f.writer() << clock.clocks_[i] << " ";
    }
}

#endif // __VECTOR_CLOCK_HPP__
