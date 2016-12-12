#include <string>
#include <vector>

#include <boost/program_options.hpp>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include "constant.hpp"
#include "environment.hpp"
#include "stack.hpp"
#include "linked_list.hpp"

namespace po = boost::program_options;
using namespace std::literals::string_literals;

class option {
public:
    std::string test_case;
    std::string test_type;
    size_t seed = 0;
    int print_idx = -1;
};

void run_stack(const option& opt, environment& env)
{
    stack s;
    int cnt = 0;
    int missed = 0;
    bool ended = false;
    constexpr int iter_cnt = 2;

    env.register_thread(0, [&] {
        for (int i = 0; i < iter_cnt; i++) {
            s.push(i);
        }
        ended = true;
    });
    env.register_thread(1, [&] {
        while (cnt < iter_cnt) {
            auto a = s.pop();
            if (a != -1) {
                cnt++;
            }
            else if (ended) {
                break;
            }
        }
    });
    env.register_thread(2, [&] {
        while (cnt < iter_cnt) {
            auto a = s.pop();
            if (a != -1) {
                cnt++;
            }
            else if (ended) {
                break;
            }
        }
    });
    bool result = env.run();    
    if (cnt != iter_cnt) {
        env.error_ = "Wrong result";
    }
}

void run_list(const option& opt, environment& env)
{
    linked_list l;
    constexpr int iter_cnt = 2;
    bool removed[iter_cnt] = {
        false,
    };
    int removed_cnt = 0;

    env.register_thread(0, [&] {
        for (int i = 0; i < iter_cnt; i++) {
            l.insert(i);
            l.check_invariant();
        }
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
    });
    env.register_thread(2, [&] {
        for (int i = 0; i < iter_cnt; i++) {
            l.find(i);
            l.check_invariant();
        }
    });
    env.run();
}

void run(int idx, const option& opt, std::unique_ptr<scheduler> sched, std::bitset<MaxPathBucket>& cov)
{
    auto& env = environment::reset();
    if (idx == opt.print_idx) {
        env.enable_logging_ = true;
    }
    env.scheduler_ = std::move(sched);
    if (opt.test_case == "list") {
        run_list(opt, env);
    } else if (opt.test_case == "stack") {
        run_stack(opt, env);
    }
    size_t path_cov = env.history_.back().coverage_;

    for (auto& th : env.threads_) {
        for (auto& fn : th.coverage_) {
            cov |= fn.path_;
        }
    }
    if (opt.print_idx == -1) {
        fmt::print("[{}]\t{}\t\t{}\t\t{}\n", idx, path_cov, cov.count(), env.error_);
    }
}

void run_random(const option& opt)
{
    if (opt.print_idx == -1) {
        fmt::print("\tpath_cov\taccum_cov\terror\n");
        fmt::print("-----------------------------------------\n");
    }
    std::mt19937_64 rnd{ opt.seed };
    std::bitset<MaxPathBucket> coverage;
    for (int i = 0; i < 100; i++) {
        run(i, opt, std::make_unique<random_scheduler>(rnd()), coverage);
    }
}

std::vector<int> mutate_history(const std::vector<history>& input, std::mt19937_64& rnd) {
    std::vector<int> mutated;
    for (auto& i : input) {
        mutated.push_back(i.thread_);
    }
    std::uniform_int_distribution<size_t> pick(0, input.size()-1);
    mutated.resize(pick(rnd));
    return mutated;
}

void run_with_generated_history(const option& opt) {
    if (opt.print_idx == -1) {
        fmt::print("\tpath_cov\taccum_cov\terror\n");
        fmt::print("----------------------------------\n");
    }
    std::mt19937_64 rnd{ opt.seed };
    std::vector<std::vector<history>> current;
    std::bitset<MaxPathBucket> coverage;
    int idx = 0; 
    for (int i = 0; i < 10; i++) {
        run(idx++, opt, std::make_unique<prefix_scheduler>(rnd()), coverage);
        auto& env = environment::get();
        current.emplace_back(env.history_);
    }

    for (int iter = 0; iter < 9; iter++) {
        for (int i = 0; i < 10; i++) {
            run(idx++, opt, std::make_unique<prefix_scheduler>(rnd(), mutate_history(current[i], rnd)), coverage);
            auto& env = environment::get();
            current.emplace_back(env.history_);
        }

        std::sort(current.begin(), current.end(), [](const auto& l, const auto& r) {
            return l.back().coverage_ > r.back().coverage_;
        });

        current.resize(10);
    }
}

int main(int argc, char** argv)
{
    option opt;
    try {
        po::options_description desc{ "Options" };
        desc.add_options()("help,h", "Help screen")
                          ("case", po::value<std::string>(), "Test case")
                          ("type", po::value<std::string>()->default_value("random"), "Test method")
                          ("seed", po::value<size_t>()->default_value(0), "Random seed")
                          ("print", po::value<int>()->default_value(-1), "Print a log for the specific iteration");

        po::variables_map vm;
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);

        if (vm.count("help")) {
            fmt::print("{}", desc);
            return 0;
        }

        if (vm.count("case")) {
            opt.test_case = vm["case"].as<std::string>();
        } else {
            fmt::print("A case/test_case option is mandatory.\n");
            return 0;
        }

        if (vm.count("type")) {
            opt.test_type = vm["type"].as<std::string>();
        }

        if (vm.count("seed")) {
            opt.seed = vm["seed"].as<size_t>();
        }

        if (vm.count("print")) {
            opt.print_idx = vm["print"].as<int>();
        }
    }
    catch (const po::error& ex) {
        fmt::print("{}\n", ex.what());
    }

    if (opt.test_type == "random") { 
        run_random(opt);
    } else if (opt.test_type == "search") {
        run_with_generated_history(opt);
    }
}
