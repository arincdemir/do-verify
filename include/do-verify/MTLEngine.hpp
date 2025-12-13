#pragma once

#include <vector>
#include <algorithm>
#include "do-verify/interval_set.hpp"
#include <limits>

#define B_INFINITY std::numeric_limits<int>::max()

namespace do_verify {

enum class NodeType {
    PROPOSITION,
    AND,
    OR,
    NOT,
    IMPLIES,
    EVENTUALLY,
    ALWAYS,
    SINCE,
    TEST,
};

struct DenseNode {
    db_interval_set::IntervalSet state;
    db_interval_set::IntervalSet output;
    NodeType type;
    unsigned int leftOperandIndex;
    unsigned int rightOperandIndex;
    int a;
    int b;
};

int add_with_inf(int a, int b);

db_interval_set::IntervalSet run_evaluation(std::vector<DenseNode> &nodes, db_interval_set::IntervalSetHolder &setHolder, const int startTime, const int endTime, const std::vector<bool> &propositionInputs);


struct DiscreteNode {
    db_interval_set::IntervalSet state;
    bool output;
    NodeType type;
    unsigned int leftOperandIndex;
    unsigned int rightOperandIndex;
    int a;
    int b;
};


bool run_evaluation(std::vector<DiscreteNode> &nodes, db_interval_set::IntervalSetHolder &setHolder, const int time, const std::vector<bool> &propositionInputs);

} // namespace do_verify
