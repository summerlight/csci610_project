#include "scheduler.hpp"

#include <fmt/format.h>
#include <fmt/ostream.h>
#include <range/v3/algorithm/shuffle.hpp>
#include "constant.hpp"

void random_scheduler::ready(int thread_id)
{
    //fmt::print("random_scheduler::ready({});\n", thread_id);
    active_.emplace_back(thread_id);
    ranges::shuffle(active_);
}

int random_scheduler::next()
{
    if (active_.empty()) {
        return -1;
    }
    int ret = active_.back();
    active_.pop_back();

    //fmt::print("random_scheduler::next -> {};\n", ret);
    return ret;
}