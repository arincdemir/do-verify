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




TEST_CASE("Discrete AbsentAQ", "[discrete][AbsentAQ]") {

    using namespace db_interval_set;
    using namespace do_verify;


     // --- 1. SETUP PARAMS ---
    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    // Unpack parameters
    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AbsentAQ " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, TIMINGS};
        DiscreteNode notNode{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};
        DiscreteNode since{empty(holder), false, NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DiscreteNode implies{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{q, p, once, notNode, since, implies, always};

        std::string file_name = "data/fullsuite/AbsentAQ/" + CONDENSATION + "/1M/AbsentAQ" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;
        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);

    };
}



TEST_CASE("Discrete AbsentBQR", "[discrete][AbsentBQR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AbsentBQR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int since_a = 3 * (TIMINGS / 10);
        const int since_b = TIMINGS;

        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};          
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; 
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};           
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};           
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};          
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 7, 0, since_a, since_b}; 
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 8, 0, 0}; 
        DiscreteNode always_node{empty(holder), false, NodeType::ALWAYS, 0, 9, 0, B_INFINITY}; 

        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        std::string file_name = "data/fullsuite/AbsentBQR/" + CONDENSATION + "/1M/AbsentBQR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "AbsentBQR Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}

TEST_CASE("Discrete AbsentBR", "[discrete][AbsentBR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AbsentBR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int inner_always_b = TIMINGS;

        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};          
        DiscreteNode inner_always{empty(holder), false, NodeType::ALWAYS, 0, 3, 0, inner_always_b}; 
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 2, 4, 0, 0}; 
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY}; 

        std::vector<DiscreteNode> nodes{q, p, r, not_p, inner_always, implies_node, root_always};

        std::string file_name = "data/fullsuite/AbsentBR/" + CONDENSATION + "/1M/AbsentBR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "AbsentBR Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}

TEST_CASE("Discrete AlwaysAQ", "[discrete][AlwaysAQ]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AlwaysAQ " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);
        
        const int once_b = TIMINGS;

        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, once_b}; 
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 1, 0, 0, B_INFINITY}; 
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 3, 4, 0, 0}; 
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 5, 0, B_INFINITY}; 

        std::vector<DiscreteNode> nodes{q, p, r, once_q, since_node, implies_node, root_always};

        std::string file_name = "data/fullsuite/AlwaysAQ/" + CONDENSATION + "/1M/AlwaysAQ" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "AlwaysAQ Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}

TEST_CASE("Discrete AlwaysBQR", "[discrete][AlwaysBQR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AlwaysBQR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int since_a = 3 * (TIMINGS / 10);
        const int since_b = TIMINGS;

        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};          
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; 
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};           
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};           
        DiscreteNode not_p{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};          
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 1, 0, since_a, since_b}; 
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 8, 0, 0}; 
        DiscreteNode always_node{empty(holder), false, NodeType::ALWAYS, 0, 9, 0, B_INFINITY}; 

        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        std::string file_name = "data/fullsuite/AlwaysBQR/" + CONDENSATION + "/1M/AlwaysBQR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "AlwaysBQR Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}

TEST_CASE("Discrete AlwaysBR", "[discrete][AlwaysBR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AlwaysBR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int inner_always_b = TIMINGS;

        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode inner_always{empty(holder), false, NodeType::ALWAYS, 0, 1, 0, inner_always_b}; 
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 2, 3, 0, 0}; 
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 4, 0, B_INFINITY}; 

        std::vector<DiscreteNode> nodes{q, p, r, inner_always, implies_node, root_always};

        std::string file_name = "data/fullsuite/AlwaysBR/" + CONDENSATION + "/1M/AlwaysBR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "AlwaysBR Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}

TEST_CASE("Discrete RecurBQR", "[discrete][RecurBQR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("RecurBQR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int inner_once_b = TIMINGS;

        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};          
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; 
        DiscreteNode and1{empty(holder), false, NodeType::AND, 2, 3, 0, 0};           
        DiscreteNode and2{empty(holder), false, NodeType::AND, 5, 4, 0, 0};           
        DiscreteNode p_or_q{empty(holder), false, NodeType::OR, 1, 0, 0, 0};          
        DiscreteNode once_p_or_q{empty(holder), false, NodeType::EVENTUALLY, 0, 7, 0, inner_once_b}; 
        DiscreteNode since_node{empty(holder), false, NodeType::SINCE, 8, 0, 0, B_INFINITY}; 
        DiscreteNode implies_node{empty(holder), false, NodeType::IMPLIES, 6, 9, 0, 0}; 
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 10, 0, B_INFINITY}; 

        std::vector<DiscreteNode> nodes{q, p, r, not_q, once_q, and1, and2, p_or_q, once_p_or_q, since_node, implies_node, root_always};

        std::string file_name = "data/fullsuite/RecurBQR/" + CONDENSATION + "/1M/RecurBQR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].r});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "RecurBQR Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}

TEST_CASE("Discrete RecurGLB", "[discrete][RecurGLB]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("RecurGLB " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};
        DiscreteNode once{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, TIMINGS};
        DiscreteNode always{empty(holder), false, NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<DiscreteNode> nodes{p, once, always};

        std::string file_name = "data/fullsuite/RecurGLB/" + CONDENSATION + "/1M/RecurGLB" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].p});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "RecurGLB Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}

TEST_CASE("Discrete RespondBQR", "[discrete][RespondBQR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("RespondBQR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int once_a = 3 * (TIMINGS / 10);
        const int once_b = TIMINGS;
        const int since_a = TIMINGS;

        DiscreteNode q{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode s{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode r{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode not_q{empty(holder), false, NodeType::NOT, 0, 0, 0, 0};          
        DiscreteNode once_q{empty(holder), false, NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; 
        DiscreteNode and_A1{empty(holder), false, NodeType::AND, 3, 4, 0, 0};         
        DiscreteNode and_A2{empty(holder), false, NodeType::AND, 6, 5, 0, 0};         
        DiscreteNode once_p{empty(holder), false, NodeType::EVENTUALLY, 0, 1, once_a, once_b}; 
        DiscreteNode implies_D{empty(holder), false, NodeType::IMPLIES, 2, 8, 0, 0};    
        DiscreteNode not_s{empty(holder), false, NodeType::NOT, 0, 2, 0, 0};          
        DiscreteNode since_F{empty(holder), false, NodeType::SINCE, 10, 1, since_a, B_INFINITY}; 
        DiscreteNode not_F{empty(holder), false, NodeType::NOT, 0, 11, 0, 0};         
        DiscreteNode and_C{empty(holder), false, NodeType::AND, 9, 12, 0, 0};         
        DiscreteNode since_B{empty(holder), false, NodeType::SINCE, 13, 0, 0, B_INFINITY}; 
        DiscreteNode implies_main{empty(holder), false, NodeType::IMPLIES, 7, 14, 0, 0}; 
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 15, 0, B_INFINITY}; 

        std::vector<DiscreteNode> nodes{q, p, s, r, not_q, once_q, and_A1, and_A2, once_p, implies_D, not_s, since_F, not_F, and_C, since_B, implies_main, root_always};

        std::string file_name = "data/fullsuite/RespondBQR/" + CONDENSATION + "/1M/RespondBQR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].q, allInputs[i].p, allInputs[i].s, allInputs[i].r});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "RespondBQR Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}

TEST_CASE("Discrete RespondGLB", "[discrete][RespondGLB]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Discrete", 10}, 
        {"Discrete", 100}, 
        {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("RespondGLB " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int once_a = 3 * (TIMINGS / 10);
        const int once_b = TIMINGS;
        const int since_a = TIMINGS;

        DiscreteNode p{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode s{empty(holder), false, NodeType::PROPOSITION, 0, 0, 0, 0};      
        DiscreteNode once_p{empty(holder), false, NodeType::EVENTUALLY, 0, 0, once_a, once_b}; 
        DiscreteNode implies_D{empty(holder), false, NodeType::IMPLIES, 1, 2, 0, 0};    
        DiscreteNode not_s{empty(holder), false, NodeType::NOT, 0, 1, 0, 0};          
        DiscreteNode since_F{empty(holder), false, NodeType::SINCE, 4, 0, since_a, B_INFINITY}; 
        DiscreteNode not_F{empty(holder), false, NodeType::NOT, 0, 5, 0, 0};         
        DiscreteNode and_C{empty(holder), false, NodeType::AND, 3, 6, 0, 0};         
        DiscreteNode root_always{empty(holder), false, NodeType::ALWAYS, 0, 7, 0, B_INFINITY}; 

        std::vector<DiscreteNode> nodes{p, s, once_p, implies_D, not_s, since_F, not_F, and_C, root_always};

        std::string file_name = "data/fullsuite/RespondGLB/" + CONDENSATION + "/1M/RespondGLB" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        bool finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 0; i < allInputs.size(); i++){
            bool output = run_evaluation(nodes, holder, allInputs[i].time, {allInputs[i].p, allInputs[i].s});
            if (!output) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
            }
            maxHolderUsage = std::max(maxHolderUsage, holder.writeIndex);
            finalOutput = output;
            swapBuffers(holder);
        }
        std::cout << "RespondGLB Usage: " << maxHolderUsage << std::endl;
        REQUIRE(all_correct == true);
        destroyHolder(holder);
    };
}