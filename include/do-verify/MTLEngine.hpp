#pragma once

#include <vector>
#include <algorithm>
#include "do-verify/interval_set.hpp"
#include <string>
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

struct ParsedNode {
    NodeType type;
    unsigned int leftOperandIndex;  // For propositions, this is the input index
    unsigned int rightOperandIndex; // unary operator operand sits here
    int a;
    int b;
};

struct DenseNode {
    db_interval_set::IntervalSet state;
    db_interval_set::IntervalSet output;
    NodeType type;
    unsigned int leftOperandIndex;
    unsigned int rightOperandIndex; // unary operator operand sits here
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
    unsigned int rightOperandIndex; // unary operator operand sits here
    int a;
    int b;
};


bool run_evaluation(std::vector<DiscreteNode> &nodes, db_interval_set::IntervalSetHolder &setHolder, const int time, const std::vector<bool> &propositionInputs);


/*
struct DenseMonitor {
    std::vector<DenseNode> nodes;
    db_interval_set::IntervalSetHolder setHolder;
    std::map<std::string, unsigned int> proposition_map;

    public:

    DenseMonitor(std::string pattern, std::vector<std::string> proposition_names_in_input_order) {
        ptl_parser parser;
        setHolder = db_interval_set::newHolder(1000);
        nodes = parser.parse_dense(pattern, setHolder);
        proposition_map = parser.get_proposition_map(); 
        // Here we change the leftOperandIndex's of Proposition nodes to their respective input indexes.
        // TODO: They will use that in evaluation to get their boolean value from the inputs
        for (int i = 0; i < proposition_names_in_input_order.size(); i++) {
            nodes[proposition_map[proposition_names_in_input_order[i]]].leftOperandIndex = i;
        }
    }

    db_interval_set::IntervalSet evaluate(const int startTime, const int endTime, const std::vector<bool> &propositionInputs) {
        return run_evaluation(nodes, setHolder, startTime, endTime, propositionInputs);
    }
};
*/

} // namespace do_verify
