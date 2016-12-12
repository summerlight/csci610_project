#include <fmt/format.h>
#include <fmt/ostream.h>
#include <boost/functional/hash.hpp>
#include "instrument.hpp"
#include "environment.hpp"

void code_coverage::covered(int block_no) {
    boost::hash_combine(state, block_no);
    block_.set(block_no);
    path_.set(state % MaxPathBucket);
    //fmt::print("[instrument] func {}, block {} -> state {}\n", func_no, block_no, state % MaxPathBucket);
}

void code_coverage::reset() {
    state = thread_id * MaxFuncCnt + func_no;
}

extern "C" void
__instrument(int func_no, int block_no)
{
    environment::get().this_thread().coverage_[func_no].covered(block_no);
}

extern "C" void __instrument_reset(int func_no)
{
    //fmt::print("[instrument] reset func {}\n", func_no);
    environment::get().this_thread().coverage_[func_no].reset();
}