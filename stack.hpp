#ifndef __STACK_HPP__
#define __STACK_HPP__

#include "var.hpp"

class stack {
    struct node {
        int value;
        var_ptr<node> next{ nullptr };
    };

    var_ptr<node> head_{ nullptr };

public:
    void push(int value);
    int pop();
};

#endif