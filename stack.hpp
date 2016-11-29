#ifndef __STACK_HPP__
#define __STACK_HPP__

#include "var.hpp"

class stack {
    struct node {
        int value;
        var<node*, 4> next{ nullptr };
    };

    atomic<node*, 4> head_{ nullptr };

public:
    void push(int value);
    int pop();
};

#endif