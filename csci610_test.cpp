#define CATCH_CONFIG_MAIN
#include <thread>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <catch.hpp>

#include "thread.hpp"
#include "scheduler.hpp"
#include "var.hpp"

namespace fb = boost::fibers;

TEST_CASE("datarace test0", "[datarace]")
{
    thread_context<2> a{ 0 };
    thread_context<2> b{ 1 };
    var<int, 2> x{ 0, a };
    bool ex = false;

    fb::fiber f1([&] {
        try {
            auto tmp = x.load(a);
            boost::this_fiber::yield();
            x.store(a, tmp + 1);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });

    fb::fiber f2([&] {
        try {
            auto tmp = x.load(b);
            boost::this_fiber::yield();
            x.store(b, tmp + 1);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });

    f1.join();
    f2.join();
    REQUIRE(ex);
}

TEST_CASE("datarace test1", "[datarace]")
{
    thread_context<2> a{ 0 };
    thread_context<2> b{ 1 };
    var<int, 2> x{ 0, a };
    var<int, 2> y{ 0, b };
    bool ex = false;

    fb::fiber f1([&] {
        try {
            auto tmp = x.load(a);
            boost::this_fiber::yield();
            x.store(a, tmp + 1);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });

    fb::fiber f2([&] {
        try {
            auto tmp = y.load(b);
            boost::this_fiber::yield();
            y.store(b, tmp + 1);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });
    f1.join();
    f2.join();
    REQUIRE(!ex);
}

TEST_CASE("datarace test2", "[datarace]")
{
    thread_context<2> a{ 0 };
    thread_context<2> b{ 1 };
    uint32_t seq_cst[2] = {
        0,
    };
    var<int, 2> x{ 0, a };
    bool ex = false;

    fb::fiber f1([&] {
        try {
            a.fence_seq_cst(seq_cst);
            auto tmp = x.load(a);
            x.store(a, tmp + 1);
            a.fence_seq_cst(seq_cst);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });

    fb::fiber f2([&] {
        try {
            b.fence_seq_cst(seq_cst);
            auto tmp = x.load(b);
            x.store(b, tmp + 1);
            b.fence_seq_cst(seq_cst);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });
    f1.join();
    f2.join();
    REQUIRE(!ex);
}

TEST_CASE("datarace test3", "[datarace]")
{
    thread_context<2> a{ 0 };
    thread_context<2> b{ 1 };
    var<int, 2> x{ 0, a };
    bool ex = false;

    fb::fiber f1([&] {
        try {
            x.store(b, 1);
            b.fence_release();
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });

    fb::fiber f2([&] {
        try {
            a.fence_acquire();
            auto tmp = x.load(a);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });
    f1.join();
    f2.join();
    REQUIRE(ex);
}

TEST_CASE("datarace test4", "[datarace]")
{
    thread_context<2> a{ 0 };
    thread_context<2> b{ 1 };
    uint32_t seq_cst[2] = {
        0,
    };
    var<int, 2> x{ 0, a };
    bool ex = false;

    fb::fiber f1([&] {
        try {
            a.fence_seq_cst(seq_cst);
            auto tmp = x.load(a);
            boost::this_fiber::yield();
            x.store(a, tmp + 1);
            a.fence_seq_cst(seq_cst);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });

    fb::fiber f2([&] {
        try {
            b.fence_seq_cst(seq_cst);
            auto tmp = x.load(b);
            boost::this_fiber::yield();
            x.store(b, tmp + 1);
            b.fence_seq_cst(seq_cst);
        }
        catch (std::runtime_error&) {
            ex = true;
        }
    });
    f1.join();
    f2.join();
    REQUIRE(ex);
}