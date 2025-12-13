#include "do-verify/MTLEngine.hpp"

namespace do_verify {

int add_with_inf(int a, int b) {
    if (a == B_INFINITY || b == B_INFINITY) {
        return B_INFINITY;
    }
    else {
        return a + b;
    }
}

db_interval_set::IntervalSet run_evaluation(std::vector<DenseNode> &nodes, db_interval_set::IntervalSetHolder &setHolder, const int startTime, const int endTime, const std::vector<bool> &propositionInputs) {
    for(size_t node_index = 0; node_index < nodes.size(); node_index++) {
        DenseNode &curNode = nodes[node_index];
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
                    (void)a; // Suppress unused variable warning
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


bool run_evaluation(std::vector<DiscreteNode> &nodes, db_interval_set::IntervalSetHolder &setHolder, const int time, const std::vector<bool> &propositionInputs) {
    for (unsigned int node_index = 0; node_index < nodes.size(); node_index++) {
        DiscreteNode &curNode = nodes[node_index];
        switch (curNode.type)
        {
        case NodeType::PROPOSITION:
            curNode.output = propositionInputs[node_index];
            break;
        case NodeType::AND:
            curNode.output = nodes[curNode.leftOperandIndex].output && nodes[curNode.rightOperandIndex].output;
            break;
        case NodeType::OR:
            curNode.output = nodes[curNode.leftOperandIndex].output || nodes[curNode.rightOperandIndex].output;
            break;
        case NodeType::NOT:
            curNode.output = !nodes[curNode.rightOperandIndex].output;
            break;    
        case NodeType::IMPLIES:
            curNode.output = !(nodes[curNode.leftOperandIndex].output && !nodes[curNode.rightOperandIndex].output);
            break; 
        case NodeType::EVENTUALLY:
        {
            if (nodes[curNode.rightOperandIndex].output) {
                curNode.state = db_interval_set::unionSets(setHolder, curNode.state,
                    db_interval_set::fromInterval(setHolder, {time + curNode.a, add_with_inf(time + 1, curNode.b)}));
            }
            curNode.output = db_interval_set::includes(curNode.state, time);
            curNode.state = db_interval_set::intersectSets(setHolder, curNode.state,
                db_interval_set::fromInterval(setHolder, {time + 1, B_INFINITY}));
            break;

        }   
        case NodeType::ALWAYS:
        {
            if (!nodes[curNode.rightOperandIndex].output) {
                curNode.state = db_interval_set::unionSets(setHolder, curNode.state,
                    db_interval_set::fromInterval(setHolder, {time + curNode.a, add_with_inf(time + 1, curNode.b)}));
            }
            curNode.output = !db_interval_set::includes(curNode.state, time);
            curNode.state = db_interval_set::intersectSets(setHolder, curNode.state,
                db_interval_set::fromInterval(setHolder, {time + 1, B_INFINITY}));
            break;
        }
        case NodeType::SINCE:
        {
            bool leftOutput = nodes[curNode.leftOperandIndex].output;
            bool rightOutput = nodes[curNode.rightOperandIndex].output;
            if (leftOutput && rightOutput) {
                curNode.state = db_interval_set::unionSets(setHolder, curNode.state,
                    db_interval_set::fromInterval(setHolder, {time + curNode.a, add_with_inf(time + 1, curNode.b)}));
            }
            else if (!leftOutput && rightOutput) {
                curNode.state = db_interval_set::fromInterval(setHolder, {time + curNode.a, add_with_inf(time + 1, curNode.b)});
            }
            else if (leftOutput && !rightOutput) {

            }
            else {
                curNode.state = db_interval_set::empty(setHolder);
            }
            curNode.output = db_interval_set::includes(curNode.state, time);
            curNode.state = db_interval_set::intersectSets(setHolder, curNode.state,
                db_interval_set::fromInterval(setHolder, {time + 1, B_INFINITY}));
            break;
        }
        case NodeType::TEST:
            break;
        }
    }
    return nodes[nodes.size() - 1].output;
    
}

} // namespace do_verify
