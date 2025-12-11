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
TEST_CASE("Dense Implementation tests", "[dense]") {
    using namespace std;
    using namespace do_verify;
    using namespace db_interval_set;
    
    auto holder = newHolder(1000);
    vector<DenseNode> nodes;
    auto p = DenseNode{empty(holder), createSetFromIntervals(holder, {{7, 30}}), NodeType::TEST, 0, 0, 0, 0};
    nodes.push_back(p);
    auto q = DenseNode{empty(holder), createSetFromIntervals(holder, {{3, 8}}), NodeType::TEST, 0, 0, 0, 0};
    nodes.push_back(q);
    auto since = DenseNode{empty(holder), empty(holder), NodeType::SINCE, 0, 1, 18, 24};
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



TEST_CASE("Dense Timescales Tests", "[dense][old]") {

    using namespace db_interval_set;
    using namespace do_verify;

    IntervalSetHolder holder = newHolder(1000);
    DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
    DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, 10};
    DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
    std::vector<DenseNode> nodes{p, once, always};

    std::ifstream input_file("data/fullsuite/RecurGLB/Dense10/1M/RecurGLB10.jsonl");
    json_reader::TimescalesInput prevInput;
    int step = 0;
    bool all_correct = true;

    for (std::string line; std::getline(input_file, line);){
        if (step != 0) {
            auto newInput = json_reader::read_line(line);
            IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
            if (toVectorIntervals(output) != std::vector<Interval>{{prevInput.time, newInput.time}}) {
                all_correct = false;
                // Optional: break early or log the specific failure step if needed
                // break; 
            }
            prevInput = newInput;
            swapBuffers(holder);
        }
        else {
            prevInput = json_reader::read_line(line);
        }
        step++;
    };

    REQUIRE(all_correct);

}



TEST_CASE("Dense AbsentAQ", "[dense][AbsentAQ]") {

    using namespace db_interval_set;
    using namespace do_verify;


     // --- 1. SETUP PARAMS ---
    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10},
        {"Dense100", 10}, 
        {"Discrete", 10}, 
        {"Dense10", 100},
        {"Dense100", 100}, 
        {"Discrete", 100}, 
        {"Dense10", 1000},
        {"Dense100", 1000}, 
        {"Discrete", 1000}, 
    }));

    // Unpack parameters
    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AbsentAQ " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, TIMINGS};
        DenseNode notNode{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        DenseNode since{empty(holder), empty(holder), NodeType::SINCE, 3, 0, 0, B_INFINITY};
        DenseNode implies{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0};
        DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<DenseNode> nodes{q, p, once, notNode, since, implies, always};

        std::string file_name = "data/fullsuite/AbsentAQ/" + CONDENSATION + "/1M/AbsentAQ" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;
        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p});
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
                all_correct = false;
                std::cout << benchmarkName << std::endl;
                break;
                // Optional: break early or log the specific failure step if needed
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


TEST_CASE("Dense AbsentBQR", "[dense][AbsentBQR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AbsentBQR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int since_a = 3 * (TIMINGS / 10);
        const int since_b = TIMINGS;

        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};          
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; 
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};           
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};           
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};          
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 7, 0, since_a, since_b}; 
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0}; 
        DenseNode always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY}; 

        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        std::string file_name = "data/fullsuite/AbsentBQR/" + CONDENSATION + "/1M/AbsentBQR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
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

TEST_CASE("Dense AbsentBR", "[dense][AbsentBR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AbsentBR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int inner_always_b = TIMINGS;

        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};          
        DenseNode inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 3, 0, inner_always_b}; 
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0}; 
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY}; 

        std::vector<DenseNode> nodes{q, p, r, not_p, inner_always, implies_node, root_always};

        std::string file_name = "data/fullsuite/AbsentBR/" + CONDENSATION + "/1M/AbsentBR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
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


TEST_CASE("Dense AlwaysAQ", "[dense][AlwaysAQ]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AlwaysAQ " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);
        
        const int once_b = TIMINGS;

        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, once_b}; 
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, 0, B_INFINITY}; 
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 3, 4, 0, 0}; 
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY}; 

        std::vector<DenseNode> nodes{q, p, r, once_q, since_node, implies_node, root_always};

        std::string file_name = "data/fullsuite/AlwaysAQ/" + CONDENSATION + "/1M/AlwaysAQ" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
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

TEST_CASE("Dense AlwaysBQR", "[dense][AlwaysBQR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AlwaysBQR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int since_a = 3 * (TIMINGS / 10);
        const int since_b = TIMINGS;

        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};          
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; 
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};           
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};           
        DenseNode not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};          
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, since_a, since_b}; 
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0}; 
        DenseNode always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY}; 

        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, not_p, since_node, implies_node, always_node};

        std::string file_name = "data/fullsuite/AlwaysBQR/" + CONDENSATION + "/1M/AlwaysBQR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
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

TEST_CASE("Dense AlwaysBR", "[dense][AlwaysBR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("AlwaysBR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int inner_always_b = TIMINGS;

        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, inner_always_b}; 
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 3, 0, 0}; 
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 4, 0, B_INFINITY}; 

        std::vector<DenseNode> nodes{q, p, r, inner_always, implies_node, root_always};

        std::string file_name = "data/fullsuite/AlwaysBR/" + CONDENSATION + "/1M/AlwaysBR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
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

TEST_CASE("Dense RecurBQR", "[dense][RecurBQR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("RecurBQR " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);

        const int inner_once_b = TIMINGS;

        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};          
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; 
        DenseNode and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};           
        DenseNode and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};           
        DenseNode p_or_q{empty(holder), empty(holder), NodeType::OR, 1, 0, 0, 0};          
        DenseNode once_p_or_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 7, 0, inner_once_b}; 
        DenseNode since_node{empty(holder), empty(holder), NodeType::SINCE, 8, 0, 0, B_INFINITY}; 
        DenseNode implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 9, 0, 0}; 
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 10, 0, B_INFINITY}; 

        std::vector<DenseNode> nodes{q, p, r, not_q, once_q, and1, and2, p_or_q, once_p_or_q, since_node, implies_node, root_always};

        std::string file_name = "data/fullsuite/RecurBQR/" + CONDENSATION + "/1M/RecurBQR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].r});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
                all_correct = false;
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

TEST_CASE("Dense RecurGLB", "[dense][RecurGLB]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("RecurGLB " + benchmarkName) {
        IntervalSetHolder holder = newHolder(1000);
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        DenseNode once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, TIMINGS};
        DenseNode always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<DenseNode> nodes{p, once, always};

        std::string file_name = "data/fullsuite/RecurGLB/" + CONDENSATION + "/1M/RecurGLB" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].p});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
                all_correct = false;
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


TEST_CASE("Dense RespondBQR", "[dense][RespondBQR]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
    }));

    std::string CONDENSATION;
    int TIMINGS;
    std::tie(CONDENSATION, TIMINGS) = params;

    std::string benchmarkName = CONDENSATION + " " + std::to_string(TIMINGS);

    SECTION("RespondBQR " + benchmarkName) {
        // Original benchmark used 1M for RespondBQR, keeping it to avoid overflow
        IntervalSetHolder holder = newHolder(1000);

        const int once_a = 3 * (TIMINGS / 10);
        const int once_b = TIMINGS;
        const int since_a = TIMINGS;

        DenseNode q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};          
        DenseNode once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; 
        DenseNode and_A1{empty(holder), empty(holder), NodeType::AND, 3, 4, 0, 0};         
        DenseNode and_A2{empty(holder), empty(holder), NodeType::AND, 6, 5, 0, 0};         
        DenseNode once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 1, once_a, once_b}; 
        DenseNode implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 2, 8, 0, 0};    
        DenseNode not_s{empty(holder), empty(holder), NodeType::NOT, 0, 2, 0, 0};          
        DenseNode since_F{empty(holder), empty(holder), NodeType::SINCE, 10, 1, since_a, B_INFINITY}; 
        DenseNode not_F{empty(holder), empty(holder), NodeType::NOT, 0, 11, 0, 0};         
        DenseNode and_C{empty(holder), empty(holder), NodeType::AND, 9, 12, 0, 0};         
        DenseNode since_B{empty(holder), empty(holder), NodeType::SINCE, 13, 0, 0, B_INFINITY}; 
        DenseNode implies_main{empty(holder), empty(holder), NodeType::IMPLIES, 7, 14, 0, 0}; 
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 15, 0, B_INFINITY}; 

        std::vector<DenseNode> nodes{q, p, s, r, not_q, once_q, and_A1, and_A2, once_p, implies_D, not_s, since_F, not_F, and_C, since_B, implies_main, root_always};

        std::string file_name = "data/fullsuite/RespondBQR/" + CONDENSATION + "/1M/RespondBQR" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].q, allInputs[i - 1].p, allInputs[i - 1].s, allInputs[i - 1].r});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
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

TEST_CASE("Dense RespondGLB", "[dense][RespondGLB]") {
    using namespace db_interval_set;
    using namespace do_verify;

    auto params = GENERATE(table<std::string, int>({
        {"Dense10", 10}, {"Dense100", 10}, {"Discrete", 10}, 
        {"Dense10", 100}, {"Dense100", 100}, {"Discrete", 100}, 
        {"Dense10", 1000}, {"Dense100", 1000}, {"Discrete", 1000}, 
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

        DenseNode p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      
        DenseNode once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, once_a, once_b}; 
        DenseNode implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 1, 2, 0, 0};    
        DenseNode not_s{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};          
        DenseNode since_F{empty(holder), empty(holder), NodeType::SINCE, 4, 0, since_a, B_INFINITY}; 
        DenseNode not_F{empty(holder), empty(holder), NodeType::NOT, 0, 5, 0, 0};         
        DenseNode and_C{empty(holder), empty(holder), NodeType::AND, 3, 6, 0, 0};         
        DenseNode root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 7, 0, B_INFINITY}; 

        std::vector<DenseNode> nodes{p, s, once_p, implies_D, not_s, since_F, not_F, and_C, root_always};

        std::string file_name = "data/fullsuite/RespondGLB/" + CONDENSATION + "/1M/RespondGLB" + std::to_string(TIMINGS) +".row.bin";
        const auto& allInputs = binary_row_reader::readInputFile(file_name);
        
        IntervalSet finalOutput;
        bool all_correct = true;
        int maxHolderUsage = 0;

        for (int i = 1; i < allInputs.size(); i++){
            IntervalSet output = run_evaluation(nodes, holder, allInputs[i - 1].time, allInputs[i].time, {allInputs[i - 1].p, allInputs[i - 1].s});
            
            if (toVectorIntervals(output) != std::vector<Interval>{{allInputs[i - 1].time, allInputs[i].time}}) {
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