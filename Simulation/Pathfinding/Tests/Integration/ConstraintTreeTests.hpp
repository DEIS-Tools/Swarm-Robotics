#ifndef CONSTRAINTTREE_HPP
#define CONSTRAINTTREE_HPP

#include "TestInterface.hpp"
#include "ConstraintTree.hpp"

class ConstraintTreeTests : public TestInterface{
private:
    void it_can_find_vertex_conflicts();
    void it_can_find_edge_conflicts();
    void it_can_find_swap_conflicts();
    void it_gets_sorted_in_priority_queue();
    void it_is_vertex_conflict();
    void it_is_edge_conflict();
    void it_is_swap_conflict();
public:
    void run(){// This is placed in the header to help ensure all tests are run
        it_can_find_vertex_conflicts();
        it_can_find_edge_conflicts();
        it_can_find_swap_conflicts();
        it_gets_sorted_in_priority_queue();
        it_is_vertex_conflict();
        it_is_edge_conflict();
        it_is_swap_conflict();
    }
};

#endif