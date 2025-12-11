#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>

#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include "do-verify/json_reader.hpp"
#include "do-verify/binary_row_reader.hpp"


#include "do-verify/MTLEngine.hpp"


TEST_CASE("Discrete Implementation Tests", "[discrete]") {

    SECTION("2 Eventually") {
        std::vector<std::vector<bool>> propositionInputs = {
        {true, false, false, false, false, false},    // p
        {false, false, false, false, true, false}  // q
        };
        db_interval_set::IntervalSetHolder holder = db_interval_set::newHolder(1000);
        std::vector<bool> expectedOutput = {false, false, true, true, true, false};
        std::vector<do_verify::DiscreteNode> nodes;
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::OR,
        0, 1, 0, 0});
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::EVENTUALLY,
        0, 2, 1, 2});
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::EVENTUALLY,
        0, 3, 1, 2});
        bool allCorrect = true;
        for(int time = 0; time < propositionInputs[0].size(); time++) {
            bool output = do_verify::run_evaluation(nodes, holder, time, {propositionInputs[0][time], propositionInputs[1][time]});
            allCorrect &= output == expectedOutput[time];
            db_interval_set::swapBuffers(holder);
        }
        db_interval_set::destroyHolder(holder);
        REQUIRE(allCorrect == true);
        
    }


    SECTION("Always") {
        std::vector<std::vector<bool>> propositionInputs = {
        {false, false, true, true, true, false},    // p
        };
        db_interval_set::IntervalSetHolder holder = db_interval_set::newHolder(1000);
        std::vector<bool> expectedOutput = {true, false, false, false, true, true};
        std::vector<do_verify::DiscreteNode> nodes;
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::ALWAYS,
        0, 0, 1, 2});
        bool allCorrect = true;
        for(int time = 0; time < propositionInputs[0].size(); time++) {
            bool output = do_verify::run_evaluation(nodes, holder, time, {propositionInputs[0][time]});
            allCorrect &= output == expectedOutput[time];
            db_interval_set::swapBuffers(holder);
        }
        db_interval_set::destroyHolder(holder);
        REQUIRE(allCorrect == true);
    }

    SECTION("Since") {
        std::vector<std::vector<bool>> propositionInputs = {
        {false, false, true, true, true, false},    // p
        {false, true, false, false, true, false},   // q
        };
        db_interval_set::IntervalSetHolder holder = db_interval_set::newHolder(1000);
        std::vector<bool> expectedOutput = {false, false, false, true, true, false};
        std::vector<do_verify::DiscreteNode> nodes;
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(do_verify::DiscreteNode{db_interval_set::empty(holder), false, do_verify::NodeType::SINCE,
        0, 1, 2, 3});
        bool allCorrect = true;
        for(int time = 0; time < propositionInputs[0].size(); time++) {
            bool output = do_verify::run_evaluation(nodes, holder, time, {propositionInputs[0][time], propositionInputs[1][time]});
            allCorrect &= output == expectedOutput[time];
            db_interval_set::swapBuffers(holder);
        }
        db_interval_set::destroyHolder(holder);
        REQUIRE(allCorrect == true);
    }
}
