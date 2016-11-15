#ifndef __EXECUTOR_HPP__
#define __EXECUTOR_HPP__

#include "thread.hpp"

class executor {
public:
static executor& this_thread();

};

#endif // __EXECUTOR_HPP__