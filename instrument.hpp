#ifndef __INSTRUMENT_HPP__
#define __INSTRUMENT_HPP__

#include <boost/functional/hash.hpp>

#include "constant.hpp"

class code_coverage {
public:
    int thread_id = 0;
    size_t func_no = 0;
    size_t state = 0;
    std::bitset<MaxBlockCnt> block_;
    std::bitset<MaxPathBucket> path_;

    void covered(int block_no);
    void reset();
};

#endif // __INSTRUMENT_HPP__