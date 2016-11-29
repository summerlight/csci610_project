#define CATCH_CONFIG_MAIN
#include <thread>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <catch.hpp>

#include "environment.hpp"
#include "thread.hpp"
#include "scheduler.hpp"
#include "stack.hpp"
#include "var.hpp"

namespace fb = boost::fibers;

TEST_CASE("datarace test0", "[datarace]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();
    auto& env = environment<2>::reset();
    var<int, 2> x{ 0 };

    env.register_thread(0, [&] {
        auto tmp = x.load();
        boost::this_fiber::yield();
        x.store(tmp + 1);
    });

    env.register_thread(1, [&] {
        auto tmp = x.load();
        boost::this_fiber::yield();
        x.store(tmp + 1);
    });
    REQUIRE(env.run());
}

TEST_CASE("datarace test1", "[datarace]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();
    auto& env = environment<2>::reset();
    var<int, 2> x{ 0 };
    var<int, 2> y{ 0 };

    env.register_thread(0, [&] {
        auto tmp = x.load();
        boost::this_fiber::yield();
        x.store(tmp + 1);
    });

    env.register_thread(1, [&] {
        auto tmp = y.load();
        boost::this_fiber::yield();
        y.store(tmp + 1);
    });
    REQUIRE(!env.run());
}

TEST_CASE("datarace test2", "[datarace]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();
    auto& env = environment<2>::reset();
    var<int, 2> x{ 0 };

    env.register_thread(0, [&] {
        env.fence_seq_cst();
        auto tmp = x.load();
        x.store(tmp + 1);
        env.fence_seq_cst();
    });

    env.register_thread(1, [&] {
        env.fence_seq_cst();
        auto tmp = x.load();
        x.store(tmp + 1);
        env.fence_seq_cst();
    });
    REQUIRE(!env.run());
}

TEST_CASE("datarace test3", "[datarace]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();
    auto& env = environment<2>::reset();
    var<int, 2> x{ 0 };

    env.register_thread(0, [&] {
        x.store(1);
        env.fence_release();
    });

    env.register_thread(1, [&] {
        env.fence_acquire();
        auto tmp = x.load();
    });

    REQUIRE(!env.run());
}

TEST_CASE("datarace test4", "[datarace]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();

    auto& env = environment<2>::reset();
    var<int, 2> x{ 0 };

    env.register_thread(0, [&] {
        auto tmp = x.load();
        env.fence_seq_cst();
        x.store(tmp + 1);
    });

    env.register_thread(1, [&] {
        auto tmp = x.load();
        env.fence_seq_cst();
        x.store(tmp + 1);
    });

    env.run();
    //REQUIRE(!env.run());
}

TEST_CASE("atomic test0", "[atomic]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();

    auto& env = environment<2>::reset();
    atomic<int, 2> x{ 0 };

    env.register_thread(0, [&] {
        for (int i = 0; i < 5; i++) {
            x.rmw([](int x) { return x + 1; });
        }
    });
    env.register_thread(1, [&] {
        for (int i = 0; i < 5; i++) {
            x.rmw([](int x) { return x + 1; });
        }
    });
    REQUIRE(!env.run());
    REQUIRE(x.load() == 10);
}

TEST_CASE("atomic test1", "[atomic]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();

    auto& env = environment<4>::reset();

    atomic<bool, 4> x{ false };
    atomic<bool, 4> y{ false };
    atomic<int, 4> z{ 0 };

    env.register_thread(0, [&] {
        x.store(true);
    });
    env.register_thread(1, [&] {
        y.store(true);
    });
    env.register_thread(2, [&] {
        while (!x.load()) {
        }
        if (y.load()) {
            z.rmw([](int x) { return x + 1; });
        }
    });
    env.register_thread(3, [&] {
        while (!y.load()) {
        }
        if (x.load()) {
            z.rmw([](int x) { return x + 1; });
        }
    });
    REQUIRE(!env.run());
    REQUIRE(z.raw_load() != 0);
}

TEST_CASE("atomic test2", "[atomic]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();

    auto& env = environment<4>::reset();

    var<int, 4> v{ 1 };
    atomic<bool, 4> x{ false };

    env.register_thread(0, [&] {
        while (x.rmw([](bool) { return true; })) {
        }
        v.store(1);
        x.store(false);
    });
    env.register_thread(1, [&] {
        while (x.rmw([](bool) { return true; })) {
        }
        v.store(2);
        x.store(false);
    });
    env.register_thread(2, [&] {
        while (x.rmw([](bool) { return true; })) {
        }
        v.store(3);
        x.store(false);
    });
    env.register_thread(3, [&] {
        while (x.rmw([](bool) { return true; })) {
        }
        v.store(4);
        x.store(false);
    });
    REQUIRE(!env.run());
    REQUIRE(v.raw_load() != 0);
    REQUIRE(!x.raw_load());
}

TEST_CASE("stack test0", "[stack]")
{
    boost::fibers::use_scheduling_algorithm<scheduler>();

    auto& env = environment<2>::reset();
    stack<2> s;

    env.register_thread(0, [&] {
        s.push(0);
    });
    env.register_thread(1, [&] {
        auto a = s.pop();
        fmt::print("{}\n", a);
    });
    REQUIRE(!env.run());
}