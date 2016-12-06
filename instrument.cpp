#include <fmt/format.h>
#include <fmt/ostream.h>
#include <boost/functional/hash.hpp>
#include "instrument.hpp"
#include "environment.hpp"

extern "C" void
__instrument(int func_no, int block_no)
{
    fmt::print("[instrument] func {}, block {}\n", func_no, block_no);
    environment::get().this_thread().coverage_[func_no].covered(block_no);
}

extern "C" void __instrument_reset(int func_no)
{
    fmt::print("[instrument] reset func {}\n", func_no);
    environment::get().this_thread().coverage_[func_no].new_call(block_no);
}