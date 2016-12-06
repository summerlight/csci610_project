#ifndef __FINALLY_HPP__
#define __FINALLY_HPP__

template <typename F>
struct finally_impl {
    finally_impl(F f)
        : cleanup_{ f }
    {
    }
    ~finally_impl()
    {
        cleanup_();
    }

private:
    F cleanup_;
};

template <typename F>
finally_impl<F> finally(F f)
{
    return finally_impl<F>(f);
}

#endif // __FINALLY_HPP__