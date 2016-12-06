#ifndef __INSTRUMENT_HPP__
#define __INSTRUMENT_HPP__

class code_coverage {
public:
    size_t state = 0;
    std::bitset<MaxBlockCnt> block_;
    std::bitset<MaxPathBucket> path_;

    void covered(int block_no) {
        boost::hash_combine(state, block_no);
        block_.set(block_no);
        path_.set(state % MaxPathBucket);
    }

    void new_call() {
        state = 0;
    }
};

#endif // __INSTRUMENT_HPP__