#include "scheduler.hpp"

#include <algorithm>
#include <fmt/format.h>
#include <fmt/ostream.h>
#include "constant.hpp"

random_scheduler::random_scheduler(size_t seed)
    : rnd_{ seed }
{
}

void random_scheduler::ready(int thread_id)
{
    //fmt::print("random_scheduler::ready({});\n", thread_id);
    active_.emplace_back(thread_id);
    std::shuffle(active_.begin(), active_.end(), rnd_);
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

prefix_scheduler::prefix_scheduler(size_t seed)
    : rnd_(seed)
{
}

prefix_scheduler::prefix_scheduler(size_t seed, const std::vector<int>& history)
    : rnd_(seed)
    , history_(history)
{
}

void prefix_scheduler::generate_history()
{
    size_t cnt = std::max<size_t>(history_.size(), 32);
    std::uniform_int_distribution<> pick(0, MaxThreadCnt - 1);
    for (int i = 0; i < cnt; i++) {
        history_.emplace_back(pick(rnd_));
    }
}

void prefix_scheduler::ready(int thread_id)
{
}

int prefix_scheduler::next()
{
    if (history_.size() < 32 || pos_ >= history_.size()) {
        generate_history();
    }

    //fmt::print("random_scheduler::next -> {};\n", ret);
    return history_[pos_++];
}
