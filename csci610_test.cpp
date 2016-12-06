#define CATCH_CONFIG_MAIN
#include <thread>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include <catch.hpp>
#include <boost/context/all.hpp>

#include "allocator.hpp"
#include "environment.hpp"
#include "thread.hpp"
#include "scheduler.hpp"
#include "stack.hpp"
#include "var.hpp"

TEST_CASE("execution test 0", "[execution]")
{
    auto& env = environment::reset();
    env.register_thread(0, [&] {
        fmt::print("th 0\n");
        env.yield();
        fmt::print("th 0\n");
    });
    env.register_thread(1, [&] {
        fmt::print("th 1\n");
        env.yield();
        fmt::print("th 1\n");
    });
    env.register_thread(2, [&] {
        fmt::print("th 2\n");
        env.yield();
        fmt::print("th 2\n");
    });
    REQUIRE(!env.run());
}

TEST_CASE("execution test 1", "[execution]")
{
    auto& env = environment::reset();
    env.register_thread(0, [&] {
        throw_exception("error");
    });
    REQUIRE(env.run());
}

TEST_CASE("datarace test0", "[datarace]")
{
    auto& env = environment::reset();
    var<int> x{ 0 };

    env.register_thread(0, [&] {
        auto tmp = x.load();
        env.yield();
        x.store(tmp + 1);
    });

    env.register_thread(1, [&] {
        auto tmp = x.load();
        env.yield();
        x.store(tmp + 1);
    });
    REQUIRE(env.run());
}

/*
TEST_CASE("datarace test1", "[datarace]")
{
    auto& env = environment::reset();
    var<int> x{ 0 };
    var<int> y{ 0 };

    env.register_thread(0, [&] {
        auto tmp = x.load();
        env.yield();
        x.store(tmp + 1);
    });

    env.register_thread(1, [&] {
        auto tmp = y.load();
        env.yield();
        y.store(tmp + 1);
    });
    REQUIRE(!env.run());
}

TEST_CASE("datarace test2", "[datarace]")
{
    auto& env = environment::reset();
    var<int> x{ 0 };

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
    REQUIRE(env.run());
}

TEST_CASE("datarace test3", "[datarace]")
{
    auto& env = environment::reset();
    var<int> x{ 0 };

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
*/
TEST_CASE("datarace test4", "[datarace]")
{
    auto& env = environment::reset();
    var<int> x{ 0 };

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
    auto& env = environment::reset();
    var<int> x{ 0 };

    env.register_thread(0, [&] {
        for (int i = 0; i < 5; i++) {
            x.atomic_rmw([](int x) { return x + 1; });
        }
    });
    env.register_thread(1, [&] {
        for (int i = 0; i < 5; i++) {
            x.atomic_rmw([](int x) { return x + 1; });
        }
    });
    REQUIRE(!env.run());
    REQUIRE(x.raw_load() == 10);
}

TEST_CASE("atomic test1", "[atomic]")
{
    auto& env = environment::reset();

    var<bool> x{ false };
    var<bool> y{ false };
    var<int> z{ 0 };

    env.register_thread(0, [&] {
        x.atomic_store(true);
    });
    env.register_thread(1, [&] {
        y.atomic_store(true);
    });
    env.register_thread(2, [&] {
        while (!x.atomic_load()) {
        }
        if (y.atomic_load()) {
            z.atomic_rmw([](int x) { return x + 1; });
        }
    });
    env.register_thread(3, [&] {
        while (!y.atomic_load()) {
        }
        if (x.atomic_load()) {
            z.atomic_rmw([](int x) { return x + 1; });
        }
    });
    REQUIRE(!env.run());
    REQUIRE(z.raw_load() != 0);
}

TEST_CASE("atomic test2", "[atomic]")
{
    auto& env = environment::reset();

    var<int> v{ 1 };
    var<bool> x{ false };

    env.register_thread(0, [&] {
        while (x.atomic_rmw([](bool) { return true; })) {
        }
        v.store(1);
        x.atomic_store(false);
    });
    env.register_thread(1, [&] {
        while (x.atomic_rmw([](bool) { return true; })) {
        }
        v.store(2);
        x.atomic_store(false);
    });
    env.register_thread(2, [&] {
        while (x.atomic_rmw([](bool) { return true; })) {
        }
        v.store(3);
        x.atomic_store(false);
    });
    env.register_thread(3, [&] {
        while (x.atomic_rmw([](bool) { return true; })) {
        }
        v.store(4);
        x.atomic_store(false);
    });
    REQUIRE(!env.run());
    REQUIRE(v.raw_load() != 0);
    REQUIRE(!x.raw_load());
}

TEST_CASE("stack test0", "[stack]")
{
    auto& env = environment::reset();
    stack s;
    int ret = 100;

    env.register_thread(0, [&] {
        s.push(0);
    });
    env.register_thread(1, [&] {
        ret = s.pop();
    });
    REQUIRE(!env.run());
    bool a = (ret == 0 || ret == -1);
    REQUIRE(a);
}

TEST_CASE("stack test1", "[stack]")
{
    auto& env = environment::reset();
    stack s;
    int cnt = 0;
    int missed = 0;
    bool ended = false;
    constexpr int iter_cnt = 3;

    env.register_thread(0, [&] {
        for (int i = 0; i < iter_cnt; i++) {
            s.push(i);
        }
        ended = true;
        fmt::print("end\n");
    });
    env.register_thread(1, [&] {
        while (cnt < iter_cnt) {
            auto a = s.pop();
            if (a != -1) {
                //fmt::print("{}\n", a);
                cnt++;
            }
            else if (ended) {
                break;
            }
        }
        fmt::print("end\n");
    });
    env.register_thread(2, [&] {
        while (cnt < iter_cnt) {
            auto a = s.pop();
            if (a != -1) {
                //fmt::print("{}\n", a);
                cnt++;
            }
            else if (ended) {
                break;
            }
        }
        fmt::print("end\n");
    });
    REQUIRE(!env.run());
    REQUIRE(cnt == iter_cnt);
}

TEST_CASE("memory test", "[memory]")
{
    auto& env = environment::reset();
    env.register_thread(0, [&] {
        auto* ptr = alloc<int>(0);
        free(ptr);
    });
    env.register_thread(1, [&] {
        auto* ptr = alloc<bool>(false);
        free(ptr);
    });
    REQUIRE(!env.run());
}

class linked_list {
    struct node {
        int value{ -1 };
        var_ptr<node> next{ nullptr };
    };

    node head_;

public:
    void insert(int value)
    {
        node* cur = &head_;
        node* next = cur->next.load();
        while (next != nullptr && value < next->value) {
            cur = next;
            next = cur->next.load();
        }

        node* new_node = alloc<node>();
        new_node->value = value;
        do {
            next = cur->next.load();
            new_node->next.store(next);
        } while (!cur->next.atomic_cas(next, new_node));
    }

    bool remove(int target)
    {
        node* cur = nullptr;
        node* next = nullptr;
        do {
            cur = &head_;
            next = cur->next.load();
            while (true) {
                if (next == nullptr || next->value < target) {
                    return false;
                }
                if (next->value == target) {
                    break;
                }
                cur = next;
                next = cur->next.load();
            }
        } while (!cur->next.atomic_cas(next, next->next.load()));

        free(next);
        return true;
    }

    bool find(int target)
    {
        node* cur = &head_;
        node* next = cur->next.load();
        while (true) {
            if (next == nullptr || next->value < target) {
                return false;
            }
            if (next->value == target) {
                return true;
            }
            cur = next;
            next = cur->next.load();
        }
        return false;
    }

    void check_invariant()
    {
        node* cur = &head_;
        while (cur->next.raw_load() != nullptr) {
            if (cur->value > cur->next.raw_load()->value) {
                throw_exception("invariant broken");
            }
            cur = cur->next.raw_load();
        }
    }
};

TEST_CASE("linked_list test0", "[linked_list]")
{
    auto& env = environment::reset();
    linked_list l;
    constexpr int iter_cnt = 3;
    bool removed[iter_cnt] = {
        false,
    };
    int removed_cnt = 0;

    env.register_thread(0, [&] {
        for (int i = 0; i < iter_cnt; i++) {
            l.insert(i);
            l.check_invariant();
        }
        fmt::print("end\n");
    });
    env.register_thread(1, [&] {
        while (removed_cnt != iter_cnt) {
            for (int i = 0; i < iter_cnt; i++) {
                if (l.remove(i)) {
                    removed_cnt++;
                }
                l.check_invariant();
            }
        }
        fmt::print("end\n");
    });
    env.register_thread(2, [&] {
        for (int i = 0; i < iter_cnt; i++) {
            l.find(i);
            l.check_invariant();
        }
        fmt::print("end\n");
    });
    REQUIRE(!env.run());
}