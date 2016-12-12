#include "linked_list.hpp"
#include "allocator.hpp"

void linked_list::insert(int value)
{
    node* cur = &head_;
    node* next = cur->next.load();
    while (next != nullptr && value < next->value) {
        cur = next;
        next = cur->next.load();
    }

    node* new_node = alloc<node>();
    new_node->value = value;
    do {
        next = cur->next.load();
        new_node->next.store(next);
    } while (!cur->next.atomic_cas(next, new_node));
}

bool linked_list::remove(int target)
{
    node* cur = nullptr;
    node* next = nullptr;
    do {
        cur = &head_;
        next = cur->next.load();
        while (true) {
            if (next == nullptr || next->value < target) {
                return false;
            }
            if (next->value == target) {
                break;
            }
            cur = next;
            next = cur->next.load();
        }
    } while (!cur->next.atomic_cas(next, next->next.load()));

    free(next);
    return true;
}

bool linked_list::find(int target)
{
    node* cur = &head_;
    node* next = cur->next.load();
    while (true) {
        if (next == nullptr || next->value < target) {
            return false;
        }
        if (next->value == target) {
            return true;
        }
        cur = next;
        next = cur->next.load();
    }
    return false;
}

void linked_list::check_invariant()
{
    node* cur = &head_;
    while (cur->next.raw_load() != nullptr) {
        if (cur->value > cur->next.raw_load()->value) {
            throw_exception("invariant broken");
        }
        cur = cur->next.raw_load();
    }
}