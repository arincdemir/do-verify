#include <array>
#include <cstddef>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <argp.h>
#include <sys/types.h>

#include <do-verify/binary_row_reader.hpp>
#include <do-verify/MTLEngine.hpp>

using namespace db_interval_set;
using namespace do_verify;

int main(int argc, char **argv)
{
    const auto &allInputs = binary_row_reader::readInputFile("/home/arinc/workspace/do-verify/data/fullsuite/AbsentAQ/Discrete/1M/AbsentAQ10.row.bin");
    for (int i = 0; i < 100; i++)
    {
        std::vector<bool> propositionInputs(2);
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 1, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, 10};
        DiscreteNode notNode{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since{empty(holder), false, NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DiscreteNode implies{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, once, notNode, since, implies, always};

        for (size_t i = 0; i < allInputs.size(); i++)
        {
            propositionInputs[0] = allInputs[i].q;
            propositionInputs[1] = allInputs[i].p;
            run_evaluation(nodes, holder, allInputs[i].time, propositionInputs);
            swapBuffers(holder);
        }
        destroyHolder(holder);
    }
}
