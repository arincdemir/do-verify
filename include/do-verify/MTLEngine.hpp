#pragma once

#include <vector>
#include <algorithm>
#include "do-verify/interval_set.hpp"
#include <limits>

#define B_INFINITY std::numeric_limits<int>::max()

namespace dense_vector {

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

struct Node {
    db_interval_set::IntervalSet state;
    db_interval_set::IntervalSet output;
    NodeType type;
    unsigned int leftOperandIndex;
    unsigned int rightOperandIndex;
    int a;
    int b;
};

inline int add_with_inf(int a, int b) {
    if (a == B_INFINITY || b == B_INFINITY) {
        return B_INFINITY;
    }
    else {
        return a + b;
    }
};

db_interval_set::IntervalSet run_evaluation(std::vector<Node> &nodes, db_interval_set::IntervalSetHolder &setHolder, const int startTime, const int endTime, const std::vector<bool> &propositionInputs) {
    for(int node_index = 0; node_index < nodes.size(); node_index++) {
        Node &curNode = nodes[node_index];
        switch (curNode.type)
        {           // TODO hocaya sor: datada bir time ve true, false geldiği zaman o timeden önce mi öyle sonra mı?
                    // timescales'de timeless'lar var nasıl implemente edicem

                    // once -> eventually
                    // historically -> always
                    // -> -> implies
        case NodeType::PROPOSITION:
            if (propositionInputs[node_index]) curNode.output = db_interval_set::fromInterval(setHolder, {startTime, endTime});
            else curNode.output = db_interval_set::empty(setHolder);
            break;
        case NodeType::AND:
            curNode.output = db_interval_set::intersectSets(setHolder, nodes[curNode.leftOperandIndex].output, nodes[curNode.rightOperandIndex].output);
            break;
        case NodeType::OR:
            curNode.output = db_interval_set::unionSets(setHolder, nodes[curNode.leftOperandIndex].output, nodes[curNode.rightOperandIndex].output);
            break;
        case NodeType::NOT:
            curNode.output = db_interval_set::negateSet(setHolder, nodes[curNode.rightOperandIndex].output, {startTime, endTime});
            break;    
        case NodeType::IMPLIES:
        {
            // A IMPLIES B is equivalent to (NOT A) OR B
            auto notLeft = db_interval_set::negateSet(setHolder, nodes[curNode.leftOperandIndex].output, 
                                                     {startTime, endTime});
            
            auto right = nodes[curNode.rightOperandIndex].output;
            curNode.output = db_interval_set::unionSets(setHolder, notLeft, right);
            break;
        } 
        
        case NodeType::EVENTUALLY:
            {
            auto rightOutput = nodes[curNode.rightOperandIndex].output;

            curNode.output = db_interval_set::empty(setHolder);

            auto iterator = db_interval_set::createSegmentIterator(db_interval_set::empty(setHolder), rightOutput, {startTime, endTime});
            int i = 0;
            while (db_interval_set::getNextSegment(iterator)) {
                if (iterator.interval.end == iterator.interval.start) continue;             
                if (iterator.rightTruthy) {
                    curNode.state = db_interval_set::unionSets(setHolder, curNode.state,
                            db_interval_set::fromInterval(setHolder, {iterator.interval.start + curNode.a, add_with_inf(iterator.interval.end, curNode.b)}));
                        int a = 0;
                        }
                i++;
                

                auto segmentOutput = db_interval_set::intersectSets(setHolder, curNode.state, 
                    db_interval_set::fromInterval(setHolder, {iterator.interval.start, iterator.interval.end}));
                curNode.output = db_interval_set::unionSets(setHolder, curNode.output, segmentOutput);
            
            }
            curNode.state = db_interval_set::intersectSets(setHolder, curNode.state,
            db_interval_set::fromInterval(setHolder, {endTime, B_INFINITY}));
            break;
        }
        case NodeType::ALWAYS:
            {
            auto rightOutput = nodes[curNode.rightOperandIndex].output;

            curNode.output = db_interval_set::empty(setHolder);

            auto iterator = db_interval_set::createSegmentIterator(db_interval_set::empty(setHolder), rightOutput, {startTime, endTime});
            
            while (db_interval_set::getNextSegment(iterator)) {
                if (iterator.interval.end == iterator.interval.start) continue;             
                if (!iterator.rightTruthy) {
                    curNode.state = db_interval_set::unionSets(setHolder, curNode.state,
                            db_interval_set::fromInterval(setHolder, {iterator.interval.start + curNode.a, add_with_inf(iterator.interval.end, curNode.b)}));
                }

                auto segmentOutput = db_interval_set::negateSet(setHolder, curNode.state, {iterator.interval.start, iterator.interval.end});
                curNode.output = db_interval_set::unionSets(setHolder, curNode.output, segmentOutput);
            
            }
            curNode.state = db_interval_set::intersectSets(setHolder, curNode.state,
            db_interval_set::fromInterval(setHolder, {endTime, B_INFINITY}));
            break;
        }
        case NodeType::SINCE:
            {
            auto leftOutput = nodes[curNode.leftOperandIndex].output;
            auto rightOutput = nodes[curNode.rightOperandIndex].output;
            curNode.output = db_interval_set::empty(setHolder);

            auto iterator = db_interval_set::createSegmentIterator(leftOutput, rightOutput, {startTime, endTime});
            
            while (db_interval_set::getNextSegment(iterator)) {
                if (iterator.interval.end == iterator.interval.start) continue;             
                if (iterator.leftTruthy && iterator.rightTruthy) {
                    curNode.state = db_interval_set::unionSets(setHolder, curNode.state,
                            db_interval_set::fromInterval(setHolder, {iterator.interval.start + curNode.a, add_with_inf(iterator.interval.end, curNode.b)}));
                }
                else if (!iterator.leftTruthy && iterator.rightTruthy) {
                    curNode.state = db_interval_set::fromInterval(setHolder, {iterator.interval.end + curNode.a, add_with_inf(iterator.interval.end, curNode.b)});
                }
                else if (iterator.leftTruthy && !iterator.rightTruthy) {
                }
                else {
                    curNode.state = db_interval_set::empty(setHolder);
                }

                auto segmentOutput = db_interval_set::intersectSets(setHolder, curNode.state, 
                    db_interval_set::fromInterval(setHolder, {iterator.interval.start, iterator.interval.end}));
                curNode.output = db_interval_set::unionSets(setHolder, curNode.output, segmentOutput);
            
            }
            curNode.state = db_interval_set::intersectSets(setHolder, curNode.state,
            db_interval_set::fromInterval(setHolder, {endTime, B_INFINITY}));
            break;
            }
        case NodeType::TEST:
            break;
        }
    
    }
    return nodes[nodes.size() - 1].output;
}


}