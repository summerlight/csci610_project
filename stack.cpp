#include "stack.hpp"

void stack::push(int value)
{
    node* new_node = new node;
    new_node->value = value;
    do {
        new_node->next.store(head_.load());
    } while (!head_.cas(new_node->next.load(), new_node));
}

int stack::pop()
{
    node* old_head = nullptr;
    do {
        old_head = head_.load();
        if (old_head == nullptr) {
            return -1;
        }
    } while (!head_.cas(old_head, old_head->next.load()));

    int result = old_head->value;
    delete old_head;
    return result;
}
