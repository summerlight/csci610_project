#ifndef __SCHEDULER_HPP__
#define __SCHEDULER_HPP__

#include <vector>
#include <random>

class scheduler {
public:
    virtual ~scheduler() = default;
    virtual void ready(int thread_id) = 0;
    virtual int next() = 0;
};

class random_scheduler : public scheduler {
    std::vector<int> active_;
    std::mt19937_64 rnd_{ 0 };

public:
    random_scheduler() = default;
    random_scheduler(size_t seed);
    ~random_scheduler() override = default;
    void ready(int thread_id) override;
    int next() override;
};

class prefix_scheduler : public scheduler {
    std::vector<int> history_;
    size_t pos_{ 0 };
    std::mt19937_64 rnd_{ 0 };

    void generate_history();

public:
    prefix_scheduler(size_t seed);
    prefix_scheduler(size_t seed, const std::vector<int>& history);
    ~prefix_scheduler() override = default;
    void ready(int thread_id) override;
    int next() override;
};

#endif // __SCHEDULER_HPP__