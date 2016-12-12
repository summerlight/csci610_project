#ifndef __LIST_HPP__
#define __LIST_HPP__

#include "var.hpp"

class linked_list {
    struct node {
        int value{ -1 };
        var_ptr<node> next{ nullptr };
    };

    node head_;

public:
    void insert(int value);
    bool remove(int target);
    bool find(int target);
    void check_invariant();
};

#endif // __LIST_HPP__