#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>



#include "dense_mtl/MTLEngine.hpp"
TEST_CASE("Dense Implementation tests", "[dense_tests]") {
    using namespace std;
    using namespace dense_vector;
    using namespace db_interval_set;
    
    auto holder = newHolder(10000);
    vector<Node> nodes;
    auto p = Node{empty(holder), createSetFromIntervals(holder, {{7, 30}}), NodeType::TEST, 0, 0, 0, 0};
    nodes.push_back(p);
    auto q = Node{empty(holder), createSetFromIntervals(holder, {{3, 8}}), NodeType::TEST, 0, 0, 0, 0};
    nodes.push_back(q);
    auto since = Node{empty(holder), empty(holder), NodeType::SINCE, 0, 1, 18, 24};
    nodes.push_back(since);

    auto out = run_evaluation(nodes, holder, 0, 30, {true, true});
    auto converted = toVectorIntervals(out);
    REQUIRE(converted == std::vector<Interval>{{25, 30}});
    swapBuffers(holder);

    nodes[0].output = createSetFromIntervals(holder, {{30,35},{39,47}});
    nodes[1].output = createSetFromIntervals(holder, {{38,39}});
    out = run_evaluation(nodes, holder, 30, 47, {true, true});
    converted = toVectorIntervals(out);
    REQUIRE(converted == std::vector<Interval>{{30, 32}});
    swapBuffers(holder);

    nodes[0].output = createSetFromIntervals(holder, {{47,49},{63,75}});
    nodes[1].output = createSetFromIntervals(holder, {{70,75}});
    out = run_evaluation(nodes, holder, 47, 75, {true, true});
    converted = toVectorIntervals(out);
    REQUIRE(converted == std::vector<Interval>{});

    swapBuffers(holder);

    nodes[0].output = createSetFromIntervals(holder, {{75,99}});
    nodes[1].output = createSetFromIntervals(holder, {{75,89}});
    out = run_evaluation(nodes, holder, 75, 99, {true, true});
    converted = toVectorIntervals(out);
    REQUIRE(converted == std::vector<Interval>{{88, 99}});

    swapBuffers(holder);
    destroyHolder(holder);
}


#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "json_reader.hpp"
TEST_CASE("Dense Timescales Tests", "[dense_tests]") {

    using namespace db_interval_set;
    using namespace dense_vector;

    IntervalSetHolder holder = newHolder(1000000);
    Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
    Node once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 10};
    Node always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
    std::vector<Node> nodes{p, once, always};

    std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/RecurGLB/Dense10/1M/RecurGLB10.jsonl");
    json_reader::TimescalesInput prevInput;
    int step = 0;
    for (std::string line; std::getline(input_file, line);){
        if (step != 0) {
            auto newInput = json_reader::read_line(line);
            IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
            REQUIRE(toVectorIntervals(output) == std::vector<Interval>{{prevInput.time, newInput.time}});
            prevInput = newInput;
            swapBuffers(holder);
        }
        else {
            prevInput = json_reader::read_line(line);
        }
        step++;
    };

}
