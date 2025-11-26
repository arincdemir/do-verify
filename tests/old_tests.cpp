#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>


#include <vector>
#include <entt/entt.hpp>
#include <cstdlib>

#include "branching/engineSetup.hpp"
#include "branching/temporalLogicEngine.hpp"

#include "branching_pro_seperate/engineSetup.hpp"
#include "branching_pro_seperate/temporalLogicEngine.hpp"

#include "function_pointers/engineSetup.hpp"
#include "function_pointers/temporalLogicEngine.hpp"

#include "branching_up_out_seperate/engineSetup.hpp"
#include "branching_up_out_seperate/temporalLogicEngine.hpp"

#include "memory_block/engineSetup.hpp"
#include "memory_block/temporalLogicEngine.hpp"

#include "heap_vs_entt/entt_linkedlist.hpp"
#include "heap_vs_entt/heap_linkedlist.hpp"



TEST_CASE("Benchmarks", "[benchmark_first]") {


    BENCHMARK_ADVANCED("Branching_SinceAnd")(Catch::Benchmark::Chronometer meter) {

        entt::registry registry = tle_branching::setupSinceAndRegistry();
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs;
        meter.measure([&] { return tle_branching::run_temporal_evaluation(registry, propositionInputs, outputs); });
    };

    BENCHMARK_ADVANCED("Branching_Pro_Seperate_SinceAnd")(Catch::Benchmark::Chronometer meter) {

        entt::registry registry = tle_branching_pro_seperate::setupSinceAndRegistry();
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs;
        meter.measure([&] { return tle_branching_pro_seperate::run_temporal_evaluation(registry, propositionInputs, outputs); });
    };

    BENCHMARK_ADVANCED("Func_Pointers_SinceAnd")(Catch::Benchmark::Chronometer meter) {
        
        entt::registry registry = tle_func_ptr::setupSinceAndRegistry();
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs;
        meter.measure([&] { return tle_func_ptr::run_temporal_evaluation(registry, propositionInputs, outputs); });
    };

    BENCHMARK_ADVANCED("Branching_Up_Out_SinceAnd")(Catch::Benchmark::Chronometer meter) {
        
        entt::registry registry = tle_branching_up_out_seperate::setupSinceAndRegistry();
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs;
        meter.measure([&] { return tle_branching_up_out_seperate::run_temporal_evaluation(registry, propositionInputs, outputs); });
    };

    BENCHMARK_ADVANCED("Memory_Block_SinceAnd")(Catch::Benchmark::Chronometer meter) {
        std::vector<memory_block::Node> nodes = memory_block::setupAlternatingSinceAndRegistry(2, 2);
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs(4);
        meter.measure([&] { return memory_block::run_temporal_evaluation(nodes, propositionInputs, outputs); });
    };


    BENCHMARK_ADVANCED("Branching_Chain_SinceAnd")(Catch::Benchmark::Chronometer meter) {

        entt::registry registry = tle_branching::setup1000SinceAndRegistry();
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs;
        meter.measure([&] { return tle_branching::run_temporal_evaluation(registry, propositionInputs, outputs); });
    };


    BENCHMARK_ADVANCED("Branching_Pro_Seperate_Chain_SinceAnd")(Catch::Benchmark::Chronometer meter) {

        entt::registry registry = tle_branching_pro_seperate::setup1000SinceAndRegistry();
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs;
        meter.measure([&] { return tle_branching_pro_seperate::run_temporal_evaluation(registry, propositionInputs, outputs); });
    };

    BENCHMARK_ADVANCED("Func_Pointers_Chain_SinceAnd")(Catch::Benchmark::Chronometer meter) {
        
        entt::registry registry = tle_func_ptr::setup1000SinceAndRegistry();
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs;
        meter.measure([&] { return tle_func_ptr::run_temporal_evaluation(registry, propositionInputs, outputs); });
    };

    BENCHMARK_ADVANCED("Branching_Up_Out_Chain_SinceAnd")(Catch::Benchmark::Chronometer meter) {
        
        entt::registry registry = tle_branching_up_out_seperate::setup1000SinceAndRegistry();
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs;
        meter.measure([&] { return tle_branching_up_out_seperate::run_temporal_evaluation(registry, propositionInputs, outputs); });
    };

    BENCHMARK_ADVANCED("Memory_Block_Chain_SinceAnd")(Catch::Benchmark::Chronometer meter) {
        std::vector<memory_block::Node> nodes = memory_block::setupAlternatingSinceAndRegistry(2, 998);
        int noTimeSteps = 1000;
        std::vector<std::vector<bool>> propositionInputs = {std::vector<bool>(noTimeSteps), {std::vector<bool>(noTimeSteps)}};
        for (int timeStep = 0; timeStep < noTimeSteps; timeStep++){
            propositionInputs[0][timeStep] = (rand() % 2 == 0);
            propositionInputs[1][timeStep] = (rand() % 2 == 0);
        }
        std::vector<std::vector<bool>> outputs(1000);
        meter.measure([&] { return memory_block::run_temporal_evaluation(nodes, propositionInputs, outputs); });
    };
    
}


TEST_CASE("Correctness for (p1 S p2) AND p2", "[correctness][complex]") {

    // The specific input for this test case
    std::vector<std::vector<bool>> propositionInputs = {
        {true, true, false, true, true, true},    // p1
        {false, true, true, false, true, false}  // p2
    };

    // The final expected output after all operations
    std::vector<bool> expected_final_output = {false, true, true, false, true, false};

    // Assuming your other setups produce the same logical graph,
    // you can test them all here.
    // If they are different, you'll need separate TEST_CASEs.

    SECTION("Function Pointers Implementation") {
        entt::registry registry = tle_func_ptr::setupSinceAndRegistry();
        std::vector<std::vector<bool>> outputs;

        tle_func_ptr::run_temporal_evaluation(registry, propositionInputs, outputs);

        // 3. Assert the output is correct
        // Check that the engine produced a single final output stream
        // Check that the output stream matches our manually calculated result
        REQUIRE(outputs[3] == expected_final_output);
    }


    SECTION("Branching Implementation") {
        entt::registry registry = tle_branching::setupSinceAndRegistry();
        std::vector<std::vector<bool>> outputs;

        tle_branching::run_temporal_evaluation(registry, propositionInputs, outputs);

        // 3. Assert the output is correct
        // Check that the engine produced a single final output stream
        // Check that the output stream matches our manually calculated result
        REQUIRE(outputs[3] == expected_final_output);
    }

    SECTION("Branching_Pro_Seperate Implementation") {
        entt::registry registry = tle_branching_pro_seperate::setupSinceAndRegistry();
        std::vector<std::vector<bool>> outputs;

        tle_branching_pro_seperate::run_temporal_evaluation(registry, propositionInputs, outputs);

        // 3. Assert the output is correct
        // Check that the engine produced a single final output stream
        // Check that the output stream matches our manually calculated result
        REQUIRE(outputs[3] == expected_final_output);
    }

    SECTION("Memory Block Implementation") {
        std::vector<memory_block::Node> nodes = memory_block::setupAlternatingSinceAndRegistry(2, 2);
        std::vector<std::vector<bool>> outputs(4);

        memory_block::run_temporal_evaluation(nodes, propositionInputs, outputs);

        // 3. Assert the output is correct
        // Check that the engine produced a single final output stream
        // Check that the output stream matches our manually calculated result
        REQUIRE(outputs[3] == expected_final_output);
    }
}


TEST_CASE("LinkedListBenchmarks", "[linked]") {


    BENCHMARK_ADVANCED("Entt_creation")(Catch::Benchmark::Chronometer meter) {

        entt::registry registry;
        int length = 10000;
        meter.measure([&] { return entt_linkedlist::createLinkedList(registry, length);});
    };

    BENCHMARK_ADVANCED("Heap_creation")(Catch::Benchmark::Chronometer meter) {

        entt::registry registry;
        int length = 10000;
        meter.measure([&] { return heap_linkedlist::createLinkedList(length);});
    };

    BENCHMARK_ADVANCED("Entt_iteration")(Catch::Benchmark::Chronometer meter) {

        entt::registry registry;
        int length = 10000;
        entt::entity head = entt_linkedlist::createLinkedList(registry, length);
        meter.measure([&] { return entt_linkedlist::loopOverLinkedList(registry, head);});
    };

    BENCHMARK_ADVANCED("Heap_iteration")(Catch::Benchmark::Chronometer meter) {

        entt::registry registry;
        int length = 10000;
        heap_linkedlist::Interval *head = heap_linkedlist::createLinkedList(length);
        meter.measure([&] { return heap_linkedlist::loopOverLinkedList(head);});
    };
}

#include <boost/icl/interval_set.hpp>
#include "boost_test/benchmark_functions.hpp"
#include "my_interval_set/interval_set.hpp"
#include "my_interval_set/benchmark_functions.hpp"

TEST_CASE("Interval Set Benchmarks", "[benchmark][interval]") {


    BENCHMARK_ADVANCED("Boost One Interval")(Catch::Benchmark::Chronometer meter) {

        auto nodes = boost_tests::create_nodes(100);
        
        meter.measure([&] { return boost_tests::process_nodes(10, 20, 10000, nodes, 1.0);});
    };

    BENCHMARK_ADVANCED("My Implementation One Interval")(Catch::Benchmark::Chronometer meter) {

        auto nodes = interval_set::create_nodes(100);
        
        meter.measure([&] { return interval_set::process_nodes(10, 20, 10000, nodes, 1.0);});
    };

    BENCHMARK_ADVANCED("Boost Many Intervals")(Catch::Benchmark::Chronometer meter) {

        auto nodes = boost_tests::create_nodes(100);
        
        meter.measure([&] { return boost_tests::process_nodes(1000, 1000, 10000, nodes, 0.5);});
    };

    BENCHMARK_ADVANCED("My Implementation Many Intervals")(Catch::Benchmark::Chronometer meter) {

        auto nodes = interval_set::create_nodes(100);
        
        meter.measure([&] { return interval_set::process_nodes(1000, 1000, 10000, nodes, 0.5);});
    };

}


#include "my_interval_set/interval_set.hpp" // The class we are testing

TEST_CASE("IntervalSet functionality", "[interval_set_implementation]") {
    interval_set::IntervalSet set;

    SECTION("Initial state is empty") {
        // A new set should not pop anything successfully.
        REQUIRE(set.pop_cur_time_from_left(0) == false);
        REQUIRE(set.pop_cur_time_from_left(100) == false);
    }

    SECTION("Adding and popping a single interval") {
        set.add_interval_from_right(5, 7); // Adds interval [5, 7]

        // Test the 'k' time constraint
        REQUIRE(set.pop_cur_time_from_left(4) == false); // Too early to pop

        // Pop the elements
        REQUIRE(set.pop_cur_time_from_left(5) == true);  // Pops 5, interval becomes [6, 7]
        REQUIRE(set.pop_cur_time_from_left(6) == true);  // Pops 6, interval becomes [7, 7]
        REQUIRE(set.pop_cur_time_from_left(7) == true); // Pops 7, interval becomes empty
        
        // The set should now be empty
        REQUIRE(set.pop_cur_time_from_left(7) == false);
    }

    SECTION("Adding a disjoint interval") {
        set.add_interval_from_right(2, 3);
        set.add_interval_from_right(5, 6);

        REQUIRE(set.pop_cur_time_from_left(2) == true); // pop 2
        REQUIRE(set.pop_cur_time_from_left(3) == true); // pop 3
        
        // First interval is gone, should be a gap at 4
        REQUIRE(set.pop_cur_time_from_left(4) == false);

        // We should now be able to pop from the second interval
        REQUIRE(set.pop_cur_time_from_left(5) == true); // pop 5
        REQUIRE(set.pop_cur_time_from_left(6) == true); // pop 6

        // Set is now empty
        REQUIRE(set.pop_cur_time_from_left(6) == false);
    }

    SECTION("Adding overlapping intervals should merge") {
        set.add_interval_from_right(10, 15);
        set.add_interval_from_right(14, 18); // This should merge

        // Pop until 18
        for(int k=10; k <= 18; ++k) {
            REQUIRE(set.pop_cur_time_from_left(k) == true);
        }

        // Set should now be empty
        REQUIRE(set.pop_cur_time_from_left(19) == false);
    }

    SECTION("Adding adjacent intervals should merge") {
        set.add_interval_from_right(10, 12);
        set.add_interval_from_right(13, 15); // This should merge

        for(int k=10; k <= 15; ++k) {
            REQUIRE(set.pop_cur_time_from_left(k) == true);
        }
        REQUIRE(set.pop_cur_time_from_left(16) == false);
    }
    

    SECTION("Popping fully exhausts the list") {
        set.add_interval_from_right(1, 1);
        set.add_interval_from_right(3, 3);

        REQUIRE(set.pop_cur_time_from_left(1) == true); // Pop 1, first interval removed
        REQUIRE(set.pop_cur_time_from_left(2) == false); // Gap
        REQUIRE(set.pop_cur_time_from_left(3) == true); // Pop 3, second interval removed

        // The object should be in a clean state and can be added to again
        set.add_interval_from_right(5, 5);
        REQUIRE(set.pop_cur_time_from_left(5) == true); // Pop 5
        REQUIRE(set.pop_cur_time_from_left(5) == false); // Should be empty again
    }
}

#include "mtl_vector/MTLEngine.hpp"

//#include "mtl_entt/MTLEngine.hpp"

TEST_CASE("MTL Implementation Functionality", "[mtl_implementation]") {


    /*
    SECTION("2 Eventually Entt") {
        std::vector<std::vector<bool>> propositionInputs = {
        {true, false, false, false, false, false},    // p
        {false, false, false, false, true, false}  // q
        };
        std::vector<bool> expectedOutput = {false, false, true, true, true, false};
        entt::registry registry;
        entt::entity p = registry.create();
        registry.emplace<mtl_entt::TopologicalRank>(p);
        registry.emplace<mtl_entt::Output>(p);
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::OR,
        0, 1, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::EVENTUALLY,
        0, 2, 1, 2});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::EVENTUALLY,
        0, 3, 1, 2});
        std::vector<bool> output;
        mtl_vector::run_evaluation(nodes, propositionInputs, output);
        // A new set should not pop anything successfully.
        REQUIRE(output == expectedOutput);
    }
    */
    

    SECTION("2 Eventually") {
        std::vector<std::vector<bool>> propositionInputs = {
        {true, false, false, false, false, false},    // p
        {false, false, false, false, true, false}  // q
        };
        std::vector<bool> expectedOutput = {false, false, true, true, true, false};
        std::vector<mtl_vector::Node> nodes;
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::OR,
        0, 1, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::EVENTUALLY,
        0, 2, 1, 2});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::EVENTUALLY,
        0, 3, 1, 2});
        std::vector<bool> output;
        mtl_vector::run_evaluation(nodes, propositionInputs, output);
        // A new set should not pop anything successfully.
        REQUIRE(output == expectedOutput);
    }


    SECTION("Always") {
        std::vector<std::vector<bool>> propositionInputs = {
        {false, false, true, true, true, false},    // p
        };
        std::vector<bool> expectedOutput = {true, false, false, false, true, true};
        std::vector<mtl_vector::Node> nodes;
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::ALWAYS,
        0, 0, 1, 2});
        std::vector<bool> output;
        mtl_vector::run_evaluation(nodes, propositionInputs, output);
        // A new set should not pop anything successfully.
        REQUIRE(output == expectedOutput);
    }

    SECTION("Since") {
        std::vector<std::vector<bool>> propositionInputs = {
        {false, false, true, true, true, false},    // p
        {false, true, false, false, true, false},   // q
        };
        std::vector<bool> expectedOutput = {false, false, false, true, true, false};
        std::vector<mtl_vector::Node> nodes;
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::SINCE,
        0, 1, 2, 3});
        std::vector<bool> output;
        mtl_vector::run_evaluation(nodes, propositionInputs, output);
        // A new set should not pop anything successfully.
        REQUIRE(output == expectedOutput);
    }
}

#include "parse_test_cases.hpp"

TEST_CASE("MTL Timescales", "[timescales]") {

        

    BENCHMARK_ADVANCED("RecurGLB10")(Catch::Benchmark::Chronometer meter) {
        std::vector<mtl_vector::Node> nodes;
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::PROPOSITION,
        0, 0, 0, 0});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::EVENTUALLY,
        0, 0, 0, 10});
        nodes.push_back(mtl_vector::Node{interval_set::IntervalSet(), false, mtl_vector::NodeType::ALWAYS,
        0, 1, 0, 1000000});

        std::vector<bool> output;
        meter.measure([&] { 
            std::vector<std::vector<bool>> propositionInputs = parseFile("/home/arinc/workspace/data-oriented-runtime-verif/largesuite/RecurGLB/RecurGLB10.jsonl");
            return mtl_vector::run_evaluation(nodes, propositionInputs, output);});
    };


}


#include "double_buffer_interval_sets/interval_set.hpp"
#include <ostream>

// Use the namespace for cleaner tests
using namespace db_interval_set;


TEST_CASE("Basic Set Creation and Lifecycle", "[interval_set_correctness][core]") {
    
    SECTION("newHolder and destroyHolder") {
        // This test mainly checks that the code compiles and doesn't crash.
        // True memory leak detection would require a tool like Valgrind.
        IntervalSetHolder holder = newHolder(128);
        REQUIRE(holder.writeIndex == 0);
        REQUIRE(holder.readBuffer != nullptr);
        REQUIRE(holder.writeBuffer != nullptr);
        destroyHolder(holder);
    }

    SECTION("swapBuffers") {
        IntervalSetHolder holder = newHolder(128);
        Transition* readPtr = holder.readBuffer;
        Transition* writePtr = holder.writeBuffer;
        holder.writeIndex = 5; // Set a dummy index

        swapBuffers(holder);

        // Pointers should be swapped
        REQUIRE(holder.readBuffer == writePtr);
        REQUIRE(holder.writeBuffer == readPtr);
        
        // Write index should be reset
        REQUIRE(holder.writeIndex == 0);

        destroyHolder(holder);
    }

    SECTION("empty set") {
        IntervalSetHolder holder = newHolder(128);
        IntervalSet s = empty(holder);
        
        // Use the helper to convert back to intervals
        std::vector<Interval> v = toVectorIntervals(s);
        
        REQUIRE(v.empty());
        destroyHolder(holder);
    }

    SECTION("fromInterval") {
        IntervalSetHolder holder = newHolder(128);
        
        // Test a valid interval
        IntervalSet s1 = fromInterval(holder, {10, 20});
        std::vector<Interval> v1 = toVectorIntervals(s1);
        REQUIRE(v1 == std::vector<Interval>{{10, 20}});

        // Test an empty interval
        IntervalSet s2 = fromInterval(holder, {30, 30});
        std::vector<Interval> v2 = toVectorIntervals(s2);
        REQUIRE(v2.empty());

        // Test an invalid interval
        IntervalSet s3 = fromInterval(holder, {40, 30});
        std::vector<Interval> v3 = toVectorIntervals(s3);
        REQUIRE(v3.empty());
        
        destroyHolder(holder);
    }
}


TEST_CASE("Union Operations (unionSets)", "[interval_set_correctness][union]") {
    IntervalSetHolder holder = newHolder(1024);

    // This is the core "double buffer" pattern for testing operations:
    // 1. Create all your *input* sets in the writeBuffer.
    // 2. swapBuffers() to move them to readBuffer.
    // 3. Define IntervalSet structs pointing to the correct slices in readBuffer.
    // 4. Run the operation (e.g., unionSets), which writes to writeBuffer.
    // 5. Convert the *result* (which is in writeBuffer) to a vector for checking.

    SECTION("Simple Overlap") {
        // A = {10, 20}, B = {15, 25} -> Union = {10, 25}
        auto s1_t = fromInterval(holder, {10, 20});
        auto s2_t = fromInterval(holder, {15, 25});
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = unionSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v == std::vector<Interval>{{10, 25}});
    }

    SECTION("Adjacent") {
        // A = {10, 20}, B = {20, 30} -> Union = {10, 30}
        auto s1_t = fromInterval(holder, {10, 20});
        auto s2_t = fromInterval(holder, {20, 30});
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = unionSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v == std::vector<Interval>{{10, 30}});
    }

    SECTION("Disjoint") {
        // A = {10, 20}, B = {30, 40} -> Union = {10, 20}, {30, 40}
        auto s1_t = fromInterval(holder, {10, 20});
        auto s2_t = fromInterval(holder, {30, 40});
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = unionSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v == std::vector<Interval>{{10, 20}, {30, 40}});
    }

    SECTION("Contained") {
        // A = {10, 40}, B = {20, 30} -> Union = {10, 40}
        auto s1_t = fromInterval(holder, {10, 40});
        auto s2_t = fromInterval(holder, {20, 30});
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = unionSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v == std::vector<Interval>{{10, 40}});
    }

    SECTION("With Empty Set") {
        // A = {10, 20}, B = {} -> Union = {10, 20}
        auto s1_t = fromInterval(holder, {10, 20});
        auto s2_t = empty(holder);
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = unionSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v == std::vector<Interval>{{10, 20}});
    }

    SECTION("Complex Multi-Interval Union") {
        // A = {10, 20} U {30, 40}
        // B = {15, 35}
        // Result = {10, 40}

        // 1. Create {10, 20} and {30, 40}
        auto i1_t = fromInterval(holder, {10, 20});
        auto i2_t = fromInterval(holder, {30, 40});
        swapBuffers(holder);
        
        // 2. Union them to create Set A
        IntervalSet i1 = {holder.readBuffer, i1_t.startIndex, i1_t.endIndex};
        IntervalSet i2 = {holder.readBuffer, i2_t.startIndex, i2_t.endIndex};
        auto setA_t = unionSets(holder, i1, i2); // setA_t is in writeBuffer
        
        // 3. Create Set B in writeBuffer
        auto setB_t = fromInterval(holder, {15, 35});

        // 4. Swap. Now setA and setB are both in readBuffer
        swapBuffers(holder);
        
        // 5. Define the final input sets
        IntervalSet setA = {holder.readBuffer, setA_t.startIndex, setA_t.endIndex};
        IntervalSet setB = {holder.readBuffer, setB_t.startIndex, setB_t.endIndex};
        
        // 6. Run the final operation
        IntervalSet result = unionSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v == std::vector<Interval>{{10, 40}});
    }

    destroyHolder(holder);
}

TEST_CASE("Intersection Operations (intersectSets)", "[interval_set_correctness][intersect]") {
    IntervalSetHolder holder = newHolder(1024);

    SECTION("Simple Overlap") {
        // A = {10, 20}, B = {15, 25} -> Intersect = {15, 20}
        auto s1_t = fromInterval(holder, {10, 20});
        auto s2_t = fromInterval(holder, {15, 25});
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = intersectSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v == std::vector<Interval>{{15, 20}});
    }

    SECTION("Adjacent") {
        // A = {10, 20}, B = {20, 30} -> Intersect = {}
        auto s1_t = fromInterval(holder, {10, 20});
        auto s2_t = fromInterval(holder, {20, 30});
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = intersectSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v.empty());
    }

    SECTION("Disjoint") {
        // A = {10, 20}, B = {30, 40} -> Intersect = {}
        auto s1_t = fromInterval(holder, {10, 20});
        auto s2_t = fromInterval(holder, {30, 40});
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = intersectSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v.empty());
    }

    SECTION("Contained") {
        // A = {10, 40}, B = {20, 30} -> Intersect = {20, 30}
        auto s1_t = fromInterval(holder, {10, 40});
        auto s2_t = fromInterval(holder, {20, 30});
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = intersectSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v == std::vector<Interval>{{20, 30}});
    }

    SECTION("With Empty Set") {
        // A = {10, 20}, B = {} -> Intersect = {}
        auto s1_t = fromInterval(holder, {10, 20});
        auto s2_t = empty(holder);
        swapBuffers(holder);

        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};
        IntervalSet setB = {holder.readBuffer, s2_t.startIndex, s2_t.endIndex};
        
        IntervalSet result = intersectSets(holder, setA, setB);
        std::vector<Interval> v = toVectorIntervals(result);

        REQUIRE(v.empty());
    }

    destroyHolder(holder);
}


TEST_CASE("Negation Operations (negateSet)", "[interval_set_correctness][negate]") {
    IntervalSetHolder holder = newHolder(1024);
    Interval domain = {0, 100};

    SECTION("Simple Negation") {
        // A = {10, 20}, Domain = {0, 100} -> Negate = {0, 10}, {20, 100}
        auto s1_t = fromInterval(holder, {10, 20});
        swapBuffers(holder);
        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};

        IntervalSet result = negateSet(holder, setA, domain);
        std::vector<Interval> v = toVectorIntervals(result);
        
        REQUIRE(v == std::vector<Interval>{{0, 10}, {20, 100}});
    }

    SECTION("Set touches domain start") {
        // A = {0, 10}, Domain = {0, 100} -> Negate = {10, 100}
        auto s1_t = fromInterval(holder, {0, 10});
        swapBuffers(holder);
        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};

        IntervalSet result = negateSet(holder, setA, domain);
        std::vector<Interval> v = toVectorIntervals(result);
        
        REQUIRE(v == std::vector<Interval>{{10, 100}});
    }

    SECTION("Set touches domain end") {
        // A = {90, 100}, Domain = {0, 100} -> Negate = {0, 90}
        auto s1_t = fromInterval(holder, {90, 100});
        swapBuffers(holder);
        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};

        IntervalSet result = negateSet(holder, setA, domain);
        std::vector<Interval> v = toVectorIntervals(result);
        
        REQUIRE(v == std::vector<Interval>{{0, 90}});
    }

    SECTION("Set covers domain") {
        // A = {0, 100}, Domain = {0, 100} -> Negate = {}
        auto s1_t = fromInterval(holder, {0, 100});
        swapBuffers(holder);
        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};

        IntervalSet result = negateSet(holder, setA, domain);
        std::vector<Interval> v = toVectorIntervals(result);
        
        REQUIRE(v.empty());
    }

    SECTION("Empty set") {
        // A = {}, Domain = {0, 100} -> Negate = {0, 100}
        auto s1_t = empty(holder);
        swapBuffers(holder);
        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};

        IntervalSet result = negateSet(holder, setA, domain);
        std::vector<Interval> v = toVectorIntervals(result);
        
        REQUIRE(v == std::vector<Interval>{{0, 100}});
    }

    SECTION("Set partially outside domain") {
        // A = {-10, 10}, Domain = {0, 100} -> Negate = {10, 100}
        auto s1_t = fromInterval(holder, {-10, 10});
        swapBuffers(holder);
        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};

        IntervalSet result = negateSet(holder, setA, domain);
        std::vector<Interval> v = toVectorIntervals(result);
        
        REQUIRE(v == std::vector<Interval>{{10, 100}});
    }

    SECTION("Set fully outside domain") {
        // A = {-20, -10}, Domain = {0, 100} -> Negate = {0, 100}
        auto s1_t = fromInterval(holder, {-20, -10});
        swapBuffers(holder);
        IntervalSet setA = {holder.readBuffer, s1_t.startIndex, s1_t.endIndex};

        IntervalSet result = negateSet(holder, setA, domain);
        std::vector<Interval> v = toVectorIntervals(result);
        
        REQUIRE(v == std::vector<Interval>{{0, 100}});
    }

    SECTION("Multi-interval set") {
        // A = {10, 20} U {50, 60}
        // Domain = {0, 100}
        // Negate = {0, 10}, {20, 50}, {60, 100}
        
        // 1. Create set A = {10, 20} U {50, 60}
        auto i1_t = fromInterval(holder, {10, 20});
        auto i2_t = fromInterval(holder, {50, 60});
        swapBuffers(holder);
        IntervalSet i1 = {holder.readBuffer, i1_t.startIndex, i1_t.endIndex};
        IntervalSet i2 = {holder.readBuffer, i2_t.startIndex, i2_t.endIndex};
        auto setA_t = unionSets(holder, i1, i2);
        
        // 2. Swap so setA is in readBuffer
        swapBuffers(holder);
        IntervalSet setA = {holder.readBuffer, setA_t.startIndex, setA_t.endIndex};

        // 3. Run negation
        IntervalSet result = negateSet(holder, setA, domain);
        std::vector<Interval> v = toVectorIntervals(result);
        
        REQUIRE(v == std::vector<Interval>{{0, 10}, {20, 50}, {60, 100}});
    }

    destroyHolder(holder);
}


#include <boost/icl/interval_set.hpp>
#include <boost/icl/right_open_interval.hpp> // For [start, end)

// A type alias for the Boost interval type matching yours
using BoostInterval = boost::icl::right_open_interval<int>;

// A type alias for the set
// Note: This is a complex template. We'll typedef it for sanity.
using BoostSet = boost::icl::interval_set<
    int,                     // Domain type
    std::less,               // Comparator
    BoostInterval            // The interval type
>;


// C++ Standard Library
#include <vector>
#include <random>
#include <chrono>

// --- Our code to test ---
using namespace db_interval_set;

// --- Boost ICL Setup ---
using BoostInterval = boost::icl::right_open_interval<int>;
using BoostSet = boost::icl::interval_set<int, std::less, BoostInterval>;


/**
 * @brief Generates a vector of random intervals.
 */
std::vector<Interval> generateRandomIntervals(int count, int maxStart, int maxDuration) {
    // Use a fixed seed for reproducible benchmarks
    std::mt19937 gen(1337); 
    std::uniform_int_distribution<> startDist(0, maxStart);
    std::uniform_int_distribution<> durationDist(1, maxDuration);
    
    std::vector<Interval> intervals;
    intervals.reserve(count);
    for (int i = 0; i < count; ++i) {
        int start = startDist(gen);
        int end = start + durationDist(gen);
        intervals.push_back({start, end});
    }
    return intervals;
}

BoostSet createBoostSetFromIntervals(const std::vector<Interval>& intervals) {
    BoostSet set;
    for (const auto& iv : intervals) {
        set.add(BoostInterval(iv.start, iv.end));
    }
    return set;
}


TEST_CASE("Performance Benchmarks", "[is_benchmark]") {
    // --- 1. SETUP ---
    // Generate a large set of test data.
    // This is run ONCE before all benchmark sections.
    const int N_A = 10000;
    const int N_B = 10000;
    const int TOTAL_N = N_A + N_B;
    
    auto intervalsA = generateRandomIntervals(N_A, 50000, 100);
    auto intervalsB = generateRandomIntervals(N_B, 50000, 100);
    
    // Create a very large holder for your library
    // Buffer needs to hold 2*N transitions (start/end) per set
    // plus room for intermediate results. 4 * TOTAL_N is safe.
    auto holder = newHolder(TOTAL_N * 4);

    // --- 2. PRE-BUILD SETS for Union/Intersect Tests ---
    // We do this *outside* the benchmark macros so we only
    // time the operation itself, not the setup.
    
    // Build DBSet A and B
    auto dbSetA_t = createSetFromIntervals(holder, intervalsA);
    auto dbSetB_t = createSetFromIntervals(holder, intervalsB);
    swapBuffers(holder); // Move both sets to readBuffer
    
    // Create read-only handles to them
    IntervalSet dbSetA = {holder.readBuffer, dbSetA_t.startIndex, dbSetA_t.endIndex};
    IntervalSet dbSetB = {holder.readBuffer, dbSetB_t.startIndex, dbSetB_t.endIndex};

    // Build BoostSet A and B
    BoostSet boostSetA = createBoostSetFromIntervals(intervalsA);
    BoostSet boostSetB = createBoostSetFromIntervals(intervalsB);


    // --- 3. RUN BENCHMARKS ---

    SECTION("Build (Construction)") {
        // This benchmarks O(N log N) build performance
        
        BENCHMARK("DBSet: Build Set from " + std::to_string(TOTAL_N) + " intervals") {
            // We must reset the write index each time
            holder.writeIndex = 0; 
            return createSetFromIntervals(holder, intervalsA); // Using intervalsA as stand-in
        };
        
        BENCHMARK("Boost: Build Set from " + std::to_string(TOTAL_N) + " intervals") {
            return createBoostSetFromIntervals(intervalsA);
        };
    }

    SECTION("Union") {
        // This benchmarks O(N+M) merge performance
        
        BENCHMARK("DBSet: Union (N=" + std::to_string(N_A) + ", M=" + std::to_string(N_B) + ")") {
            holder.writeIndex = 0; // Reset output buffer
            return unionSets(holder, dbSetA, dbSetB);
        };
        
        BENCHMARK("Boost: Union (N=" + std::to_string(N_A) + ", M=" + std::to_string(N_B) + ")") {
            return boostSetA | boostSetB;
        };
    }

    SECTION("Intersection") {
        // This benchmarks O(N+M) intersection performance
        
        BENCHMARK("DBSet: Intersect (N=" + std::to_string(N_A) + ", M=" + std::to_string(N_B) + ")") {
            holder.writeIndex = 0; // Reset output buffer
            return intersectSets(holder, dbSetA, dbSetB);
        };
        
        BENCHMARK("Boost: Intersect (N=" + std::to_string(N_A) + ", M=" + std::to_string(N_B) + ")") {
            return boostSetA & boostSetB;
        };
    }

    // --- 4. CLEANUP ---
    destroyHolder(holder);
}


#include "interval_set_scenario.hpp"
#include <tuple> // <-- Added for std::tie

TEST_CASE("Serial Update Benchmark", "[is_serial_benchmark]") {

    // --- 1. SETUP PARAMS ---
    auto params = GENERATE(table<int, int, int, int, int>({
        // Tiny workload (for validation)
        {50, 5, 100, 10000, 50},
        // Small workload
        {100, 10, 1000, 50000, 100}, 
        // Large workload (WARNING: NEEDS ~20GB RAM)
        // {500, 10, 5000, 50000, 100},
        // More sets, fewer intervals (WARNING: NEEDS ~10GB RAM)
        // {1000, 10, 500, 50000, 100}
    }));

    // Unpack parameters
    int NUM_SETS, STEPS, INTERVALS_PER_SET, MAX_START, MAX_DURATION;
    std::tie(NUM_SETS, STEPS, INTERVALS_PER_SET, MAX_START, MAX_DURATION) = params;

    // --- 2. DYNAMIC NAME (FIX 1) ---
    // This creates a descriptive name for the output
    std::string benchmark_name = 
        " (" + std::to_string(NUM_SETS) + " sets, " + 
        std::to_string(INTERVALS_PER_SET) + " iv/set)";

    
    // --- 3. COMMON DATA GENERATION ---
    // This is run once *per generated scenario*
    
    std::vector<bool> operations;
    std::mt19937 opGen(42); 
    std::uniform_int_distribution<> opDist(0, 1);
    for(int i = 0; i < NUM_SETS; ++i) {
        operations.push_back(opDist(opGen) == 1);
    }

    std::mt19937 dataGen(1337);
    std::vector<std::vector<Interval>> allIntervals(NUM_SETS);
    for (int i = 0; i < NUM_SETS; ++i) {
        allIntervals[i] = interval_set_scenario::generateRandomIntervals(dataGen, INTERVALS_PER_SET, MAX_START, MAX_DURATION);
    }
    
    // --- 4. CORRECT BUFFER SIZE CALCULATION ---
    // This is a O(N^2 * M) worst-case memory requirement.
    // Use long long to prevent overflow during calculation.
    const long long M = INTERVALS_PER_SET;
    const long long N = NUM_SETS;
    
    // Sum of arithmetic series: (N * (N+1) / 2) * M * 2 transitions
    const long long WORST_CASE_TRANSITIONS = M * N * (N + 1);
    
    // Add a 2x safety factor and check for overflow
    // This size can be *enormous* (e.g., 20-40GB+ for your large tests)
    const long long BUFFER_SIZE_LL = WORST_CASE_TRANSITIONS * 2;
    if (BUFFER_SIZE_LL > std::numeric_limits<int>::max()) {
         // This will still probably fail to allocate, but at least it's correct
         // In a real app, you might check if size_t is > max_alloc.
    }
    const size_t BUFFER_SIZE = static_cast<size_t>(BUFFER_SIZE_LL);

    // --- 5. RUN BENCHMARKS (FIX 2) ---
    // We use BENCHMARK_ADVANCED to reset state for each sample.
    
    BENCHMARK_ADVANCED("DBSet: Serial Update" + benchmark_name) (Catch::Benchmark::Chronometer meter) {
        // This setup code runs *per sample* but is *not timed*.
        
        // Allocation might fail here if BUFFER_SIZE is too large
        IntervalSetHolder holder = newHolder(BUFFER_SIZE); 
        
        std::vector<IntervalSet> dbSets(NUM_SETS);
        for (int i = 0; i < NUM_SETS; ++i) {
            dbSets[i] = createSetFromIntervals(holder, allIntervals[i]);
        }
        swapBuffers(holder); // Move initial data to readBuffer

        // This is the code that gets timed
        meter.measure([&] {
            interval_set_scenario::run_scenario_serial_update(dbSets, holder, STEPS, operations);
        });

        // Teardown (not timed)
        destroyHolder(holder);
    };
    
    BENCHMARK_ADVANCED("Boost: Serial Update" + benchmark_name) (Catch::Benchmark::Chronometer meter) {
        // This setup code runs *per sample* but is *not timed*.
        std::vector<BoostSet> boostSets(NUM_SETS);
        for (int i = 0; i < NUM_SETS; ++i) {
            boostSets[i] = interval_set_scenario::createBoostSetFromIntervals(allIntervals[i]);
        }

        // This is the code that gets timed
        meter.measure([&] {
            interval_set_scenario::run_scenario_boost(boostSets, STEPS, operations);
        });
        
        // (No teardown needed for Boost, vectors clean up themselves)
    };
}





// Helper struct for testing segments
struct ExpectedSegment {
    db_interval_set::Interval interval;
    bool a;
    bool b;

    // Overload operator== for Catch2 vector comparison
    bool operator==(const ExpectedSegment& other) const {
        return interval == other.interval && a == other.a && b == other.b;
    }
    
    // Friend function for printing
    friend std::ostream& operator<<(std::ostream& os, const ExpectedSegment& seg) {
        os << "Seg{ " << seg.interval << ", A:" << (seg.a ? "T" : "F")
           << ", B:" << (seg.b ? "T" : "F") << " }";
        return os;
    }
};

// Helper function to run the iteration and collect results
std::vector<ExpectedSegment> runIteration(
    db_interval_set::IntervalSet setA,
    db_interval_set::IntervalSet setB,
    db_interval_set::Interval domain)
{
    std::vector<ExpectedSegment> results;
    
    db_interval_set::SegmentIterator it = 
        db_interval_set::createSegmentIterator(setA, setB, domain);
        
    int segStart = it.interval.start;
    bool segA = it.leftTruthy;
    bool segB = it.rightTruthy;
    
    while (db_interval_set::getNextSegment(it)) {
        int segEnd = it.interval.end;
        
        // Only add non-empty segments
        if (segStart < segEnd) {
             results.push_back(ExpectedSegment{ {segStart, segEnd}, segA, segB });
        }
       
        // Update state for the next segment
        segStart = it.interval.start;
        segA = it.leftTruthy;
        segB = it.rightTruthy;
    }
    
    return results;
}


TEST_CASE("SegmentIterator tests", "[SegmentIterator]") {
    using namespace db_interval_set;
    
    IntervalSetHolder holder = newHolder(1024);

    // setA = [10, 20) U [30, 40)
    IntervalSet setA = createSetFromIntervals(holder, {
        {10, 20}, {30, 40}
    });

    // setB = [15, 35)
    IntervalSet setB = fromInterval(holder, {15, 35});
    
    // An empty set for testing
    IntervalSet setEmpty = empty(holder);

    SECTION("Full Domain Iteration [0, 50)") {
        Interval domain = {0, 50};
        std::vector<ExpectedSegment> results = runIteration(setA, setB, domain);

        std::vector<ExpectedSegment> expected = {
            { {0, 10},  false, false },
            { {10, 15}, true,  false },
            { {15, 20}, true,  true  },
            { {20, 30}, false, true  },
            { {30, 35}, true,  true  },
            { {35, 40}, true,  false },
            { {40, 50}, false, false }
        };
        
        REQUIRE(results == expected);
    }

    SECTION("Partial Domain [12, 32)") {
        Interval domain = {12, 32};
        std::vector<ExpectedSegment> results = runIteration(setA, setB, domain);

        std::vector<ExpectedSegment> expected = {
            { {12, 15}, true,  false },
            { {15, 20}, true,  true  },
            { {20, 30}, false, true  },
            { {30, 32}, true,  true  }
        };
        
        REQUIRE(results == expected);
    }

    SECTION("Domain before all intervals [0, 5)") {
        Interval domain = {0, 5};
        std::vector<ExpectedSegment> results = runIteration(setA, setB, domain);

        std::vector<ExpectedSegment> expected = {
            { {0, 5}, false, false }
        };
        
        REQUIRE(results == expected);
    }

    SECTION("Domain after all intervals [45, 55)") {
        Interval domain = {45, 55};
        std::vector<ExpectedSegment> results = runIteration(setA, setB, domain);
   
        std::vector<ExpectedSegment> expected= {
            { {45, 55}, false, false }
        };

        REQUIRE(results == expected);
    }
    
    SECTION("Empty Domain [5, 5)") {
        Interval domain = {5, 5};
        std::vector<ExpectedSegment> results = runIteration(setA, setB, domain);
        std::vector<ExpectedSegment> expected = {}; // No non-empty segments
        
        REQUIRE(results == expected);
    }

    SECTION("Iteration with an empty set") {
        Interval domain = {0, 20};
        // setA = empty, setB = [15, 35) (but we only care up to 20)
        std::vector<ExpectedSegment> results = runIteration(setEmpty, setB, domain);

        std::vector<ExpectedSegment> expected = {
            { {0, 15},  false, false },
            { {15, 20}, false, true  }
        };
        
        REQUIRE(results == expected);
    }
    
    SECTION("Iteration with two empty sets") {
        Interval domain = {0, 20};
        std::vector<ExpectedSegment> results = runIteration(setEmpty, setEmpty, domain);

        std::vector<ExpectedSegment> expected = {
            { {0, 20}, false, false }
        };
        
        REQUIRE(results == expected);
    }
    
    SECTION("Domain exactly matching a transition [10, 15)") {
        Interval domain = {10, 15};
        std::vector<ExpectedSegment> results = runIteration(setA, setB, domain);
        
        std::vector<ExpectedSegment> expected = {
            { {10, 15}, true, false }
        };
        
        REQUIRE(results == expected);
    }

    destroyHolder(holder);
}



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

#include <iostream>




TEST_CASE("Dense AbsentAQ", "[dense_benchmarks][AbsentAQ]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("AbsentAQ " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);
        Node q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        Node once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, TIMINGS};
        Node notNode{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};
        Node since{empty(holder), empty(holder), NodeType::SINCE, 3, 0, 0, B_INFINITY};
        Node implies{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0};
        Node always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY};
        std::vector<Node> nodes{q, p, once, notNode, since, implies, always};

        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/AbsentAQ/" + CONDENSATION + "/1M/AbsentAQ" + std::to_string(TIMINGS) +".jsonl");
        meter.measure([&] {  
            // Add these two lines to rewind the file for every run
            input_file.clear();  // Clear all error flags (like the eofbit)
            input_file.seekg(0); // Seek back to the beginning of the file
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;
            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    //std::cout << toVectorIntervals(output)[0].start << std::endl;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}


TEST_CASE("Dense AbsentBQR", "[dense_benchmarks][AbsentBQR]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("AbsentBQR " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);

        // --- 2. DEFINE FORMULA NODES ---
        // Formula: historically({r} && !{q} && once {q} ) -> ((not {p}) since[a:b] {q})
        // Where [a:b] scales with TIMINGS.
        // a = 3 * (TIMINGS / 10)
        // b = TIMINGS

        const int since_a = 3 * (TIMINGS / 10);
        const int since_b = TIMINGS;

        // --- NODES (in topological order) ---
        // Propositions (must be first, matching JSON input order)
        // We assume input is [q, p, r]
        Node q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 0
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 1
        Node r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 2

        // Left side of '->'
        Node not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};          // index 3: !{q} (op: 0)
        Node once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; // index 4: once {q} (op: 0)
        
        Node and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};          // index 5: {r} && !{q} (ops: 2, 3)
        Node and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};          // index 6: ({r} && !{q}) && (once {q}) (ops: 5, 4)

        // Right side of '->'
        Node not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};          // index 7: !{p} (op: 1)
        Node since_node{empty(holder), empty(holder), NodeType::SINCE, 7, 0, since_a, since_b}; // index 8: (not {p}) since[a:b] {q} (ops: 7, 0)

        // Main implication
        Node implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0}; // index 9: (left_side) -> (right_side) (ops: 6, 8)
        
        // Root node
        Node always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY}; // index 10: historically(...) (op: 9)


        // The vector of all nodes
        std::vector<Node> nodes{
            q, p, r,          // 0, 1, 2
            not_q,            // 3
            once_q,           // 4
            and1,             // 5
            and2,             // 6
            not_p,            // 7
            since_node,       // 8
            implies_node,     // 9
            always_node       // 10
        };

        // --- 3. RUN EVALUATION ---
        // This file path assumes the "RecurGLB" dataset, as in your previous example
        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/AbsentBQR/" + CONDENSATION + "/1M/AbsentBQR" + std::to_string(TIMINGS) +".jsonl");
        
        meter.measure([&] { 
            // Rewind the file for every benchmark run
            input_file.clear();
            input_file.seekg(0);
            
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;

            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    // Pass the proposition vector from the *previous* step
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}


TEST_CASE("Dense AbsentBR", "[dense_benchmarks][AbsentBR]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("AbsentBR " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);

        // --- 2. DEFINE FORMULA NODES ---
        // Formula: historically({r} -> (historically[:TIMINGS](not {p})))
        // We use TIMINGS for the [0, b] interval.
        
        const int inner_always_b = TIMINGS;

        // --- NODES (in topological order) ---
        // Propositions (must be first, matching JSON input order)
        // We assume input is [q, p, r]
        Node q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 0
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 1
        Node r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 2

        // Right side of '->'
        Node not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};          // index 3: !{p} (op: 1)
        Node inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 3, 0, inner_always_b}; // index 4: historically[:b](not {p}) (op: 3)

        // Main implication
        Node implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 4, 0, 0}; // index 5: {r} -> (right_side) (ops: 2, 4)
        
        // Root node
        Node root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY}; // index 6: historically(...) (op: 5)


        // The vector of all nodes
        std::vector<Node> nodes{
            q, p, r,          // 0, 1, 2
            not_p,            // 3
            inner_always,     // 4
            implies_node,     // 5
            root_always       // 6
        };

        // --- 3. RUN EVALUATION ---
        // Updated file path for AbsentBR dataset
        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/AbsentBR/" + CONDENSATION + "/1M/AbsentBR" + std::to_string(TIMINGS) +".jsonl");
        
        meter.measure([&] { 
            // Rewind the file for every benchmark run
            input_file.clear();
            input_file.seekg(0);
            
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;

            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    // Pass the proposition vector from the *previous* step
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}


TEST_CASE("Dense AlwaysAQ", "[dense_benchmarks][AlwaysAQ]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("AlwaysAQ " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);

        // --- 2. DEFINE FORMULA NODES ---
        // Formula: historically((once[:TIMINGS]({q})) -> ({p} since {q}))
        // We use TIMINGS for the [0, b] interval on 'once'.
        
        const int once_b = TIMINGS;

        // --- NODES (in topological order) ---
        // Propositions (must be first, matching JSON input order)
        // We assume input is [q, p, r]
        Node q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 0
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 1
        Node r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 2 (unused)

        // Left side of '->'
        Node once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, once_b}; // index 3: once[:b]({q}) (op: 0)

        // Right side of '->'
        Node since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, 0, B_INFINITY}; // index 4: {p} since {q} (ops: 1, 0)

        // Main implication
        Node implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 3, 4, 0, 0}; // index 5: (left_side) -> (right_side) (ops: 3, 4)
        
        // Root node
        Node root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 5, 0, B_INFINITY}; // index 6: historically(...) (op: 5)


        // The vector of all nodes
        std::vector<Node> nodes{
            q, p, r,          // 0, 1, 2
            once_q,           // 3
            since_node,       // 4
            implies_node,     // 5
            root_always       // 6
        };

        // --- 3. RUN EVALUATION ---
        // Updated file path for AlwaysAQ dataset
        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/AlwaysAQ/" + CONDENSATION + "/1M/AlwaysAQ" + std::to_string(TIMINGS) +".jsonl");
        
        meter.measure([&] { 
            // Rewind the file for every benchmark run
            input_file.clear();
            input_file.seekg(0);
            
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;

            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    // Pass the proposition vector from the *previous* step
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}


TEST_CASE("Dense AlwaysBQR", "[dense_benchmarks][AlwaysBQR]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("AlwaysBQR " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);

        // --- 2. DEFINE FORMULA NODES ---
        // Formula: historically(({r} && !{q} && once {q}) -> ({p} since[a:b] {q}))
        // Where [a:b] scales with TIMINGS.
        // a = 3 * (TIMINGS / 10)
        // b = TIMINGS

        const int since_a = 3 * (TIMINGS / 10);
        const int since_b = TIMINGS;

        // --- NODES (in topological order) ---
        // Propositions (must be first, matching JSON input order)
        // We assume input is [q, p, r]
        Node q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 0
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 1
        Node r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 2

        // Left side of '->'
        Node not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};          // index 3: !{q} (op: 0)
        Node once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; // index 4: once {q} (op: 0)
        
        Node and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};          // index 5: {r} && !{q} (ops: 2, 3)
        Node and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};          // index 6: ({r} && !{q}) && (once {q}) (ops: 5, 4)

        // Right side of '->'
        Node not_p{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};          // index 7: !{p} (op: 1)
        
        // --- THIS IS THE ONLY DIFFERENCE from AbsentBQR ---
        // AbsentBQR used (not {p}), this one uses {p}
        Node since_node{empty(holder), empty(holder), NodeType::SINCE, 1, 0, since_a, since_b}; // index 8: ({p}) since[a:b] {q} (ops: 1, 0)

        // Main implication
        Node implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 8, 0, 0}; // index 9: (left_side) -> (right_side) (ops: 6, 8)
        
        // Root node
        Node always_node{empty(holder), empty(holder), NodeType::ALWAYS, 0, 9, 0, B_INFINITY}; // index 10: historically(...) (op: 9)


        // The vector of all nodes
        std::vector<Node> nodes{
            q, p, r,          // 0, 1, 2
            not_q,            // 3
            once_q,           // 4
            and1,             // 5
            and2,             // 6
            not_p,            // 7 (This node is now unused, but leaving it is harmless and keeps indices consistent)
            since_node,       // 8
            implies_node,     // 9
            always_node       // 10
        };

        // --- 3. RUN EVALUATION ---
        // Updated file path for AlwaysBQR dataset
        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/AlwaysBQR/" + CONDENSATION + "/1M/AlwaysBQR" + std::to_string(TIMINGS) +".jsonl");
        
        meter.measure([&] { 
            // Rewind the file for every benchmark run
            input_file.clear();
            input_file.seekg(0);
            
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;

            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    // Pass the proposition vector from the *previous* step
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}




TEST_CASE("Dense AlwaysBR", "[dense_benchmarks][AlwaysBR]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("AlwaysBR " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);

        // --- 2. DEFINE FORMULA NODES ---
        // Formula: historically({r} -> (historically[:TIMINGS]({p})))
        // We use TIMINGS for the [0, b] interval.
        
        const int inner_always_b = TIMINGS;

        // --- NODES (in topological order) ---
        // Propositions (must be first, matching JSON input order)
        // We assume input is [q, p, r]
        Node q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 0 (unused)
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 1
        Node r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 2

        // Right side of '->'
        Node inner_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, inner_always_b}; // index 3: historically[:b]({p}) (op: 1)

        // Main implication
        Node implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 2, 3, 0, 0}; // index 4: {r} -> (right_side) (ops: 2, 3)
        
        // Root node
        Node root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 4, 0, B_INFINITY}; // index 5: historically(...) (op: 4)


        // The vector of all nodes
        std::vector<Node> nodes{
            q, p, r,          // 0, 1, 2
            inner_always,     // 3
            implies_node,     // 4
            root_always       // 5
        };

        // --- 3. RUN EVALUATION ---
        // Updated file path for AlwaysBR dataset
        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/AlwaysBR/" + CONDENSATION + "/1M/AlwaysBR" + std::to_string(TIMINGS) +".jsonl");
        
        meter.measure([&] { 
            // Rewind the file for every benchmark run
            input_file.clear();
            input_file.seekg(0);
            
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;

            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    // Pass the proposition vector from the *previous* step
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}



TEST_CASE("Dense RecurBQR", "[dense_benchmarks][RecurBQR]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("RecurBQR " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);

        // --- 2. DEFINE FORMULA NODES ---
        // Formula: historically(({r} && !{q} && once {q}) -> ((once[:TIMINGS]({p} or {q})) since {q}))
        // We use TIMINGS for the [0, b] interval on 'once'.
        
        const int inner_once_b = TIMINGS;

        // --- NODES (in topological order) ---
        // Propositions (must be first, matching JSON input order)
        // We assume input is [q, p, r]
        Node q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 0
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 1
        Node r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 2

        // Left side of '->'
        Node not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};          // index 3: !{q} (op: 0)
        Node once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; // index 4: once {q} (op: 0)
        Node and1{empty(holder), empty(holder), NodeType::AND, 2, 3, 0, 0};          // index 5: {r} && !{q} (ops: 2, 3)
        Node and2{empty(holder), empty(holder), NodeType::AND, 5, 4, 0, 0};          // index 6: ({r} && !{q}) && (once {q}) (ops: 5, 4)

        // Right side of '->'
        Node p_or_q{empty(holder), empty(holder), NodeType::OR, 1, 0, 0, 0};           // index 7: {p} or {q} (ops: 1, 0)
        Node once_p_or_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 7, 0, inner_once_b}; // index 8: once[:b]({p} or {q}) (op: 7)
        Node since_node{empty(holder), empty(holder), NodeType::SINCE, 8, 0, 0, B_INFINITY}; // index 9: (right_side_inner) since {q} (ops: 8, 0)

        // Main implication
        Node implies_node{empty(holder), empty(holder), NodeType::IMPLIES, 6, 9, 0, 0}; // index 10: (left_side) -> (right_side) (ops: 6, 9)
        
        // Root node
        Node root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 10, 0, B_INFINITY}; // index 11: historically(...) (op: 10)


        // The vector of all nodes
        std::vector<Node> nodes{
            q, p, r,          // 0, 1, 2
            not_q,            // 3
            once_q,           // 4
            and1,             // 5
            and2,             // 6
            p_or_q,           // 7
            once_p_or_q,      // 8
            since_node,       // 9
            implies_node,     // 10
            root_always       // 11
        };

        // --- 3. RUN EVALUATION ---
        // Updated file path for RecurBQR dataset
        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/RecurBQR/" + CONDENSATION + "/1M/RecurBQR" + std::to_string(TIMINGS) +".jsonl");
        
        meter.measure([&] { 
            // Rewind the file for every benchmark run
            input_file.clear();
            input_file.seekg(0);
            
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;

            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    // Pass the proposition vector from the *previous* step
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}


TEST_CASE("Dense RecurGLB", "[dense_benchmarks][RecurGLB]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("RecurGLB " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};
        Node once{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, TIMINGS};
        Node always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 1, 0, B_INFINITY};
        std::vector<Node> nodes{p, once, always};

        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/RecurGLB/" + CONDENSATION + "/1M/RecurGLB" + std::to_string(TIMINGS) +".jsonl");
        meter.measure([&] {  
            // Add these two lines to rewind the file for every run
            input_file.clear();  // Clear all error flags (like the eofbit)
            input_file.seekg(0); // Seek back to the beginning of the file
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;
            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    //std::cout << toVectorIntervals(output)[0].start << std::endl;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}



TEST_CASE("Dense RespondBQR", "[dense_benchmarks][RespondBQR]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("RespondBQR " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(1000000);

        // --- 2. DEFINE FORMULA NODES ---
        // Formula: historically(({r} && !{q} && once {q}) -> ( (({s} -> once[a1:b1] {p}) and not( not({s}) since[a2:] {p})) since {q}))
        
        // Scale intervals based on TIMINGS
        const int once_a = 3 * (TIMINGS / 10);
        const int once_b = TIMINGS;
        const int since_a = TIMINGS;

        // --- NODES (in topological order) ---
        // Propositions (must be first, matching JSON input order)
        // We assume input is [q, p, s, r]
        Node q{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 0
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 1
        Node s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 2
        Node r{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 3

        // Left side of '->': A = ({r} && !{q} && once {q})
        Node not_q{empty(holder), empty(holder), NodeType::NOT, 0, 0, 0, 0};          // index 4: !{q} (op: 0)
        Node once_q{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, 0, B_INFINITY}; // index 5: once {q} (op: 0)
        Node and_A1{empty(holder), empty(holder), NodeType::AND, 3, 4, 0, 0};         // index 6: {r} && !{q} (ops: 3, 4)
        Node and_A2{empty(holder), empty(holder), NodeType::AND, 6, 5, 0, 0};         // index 7: ({r} && !{q}) && once {q} (ops: 6, 5)

        // Right side of '->': B = (C since {q})
        // C = (D and E)
        // D = ({s} -> once[a:b] {p})
        // E = not(F)
        // F = (not({s}) since[a:] {p})

        // Sub-formula D
        Node once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 1, once_a, once_b}; // index 8: once[a:b] {p} (op: 1)
        Node implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 2, 8, 0, 0};     // index 9: {s} -> once_p (ops: 2, 8)

        // Sub-formula F
        Node not_s{empty(holder), empty(holder), NodeType::NOT, 0, 2, 0, 0};          // index 10: not({s}) (op: 2)
        Node since_F{empty(holder), empty(holder), NodeType::SINCE, 10, 1, since_a, B_INFINITY}; // index 11: not({s}) since[a:] {p} (ops: 10, 1)

        // Sub-formula E
        Node not_F{empty(holder), empty(holder), NodeType::NOT, 0, 11, 0, 0};         // index 12: not(F) (op: 11)

        // Sub-formula C
        Node and_C{empty(holder), empty(holder), NodeType::AND, 9, 12, 0, 0};         // index 13: (D and E) (ops: 9, 12)

        // Sub-formula B
        Node since_B{empty(holder), empty(holder), NodeType::SINCE, 13, 0, 0, B_INFINITY}; // index 14: (C since {q}) (ops: 13, 0)

        // Main implication: A -> B
        Node implies_main{empty(holder), empty(holder), NodeType::IMPLIES, 7, 14, 0, 0}; // index 15: (A -> B) (ops: 7, 14)
        
        // Root node
        Node root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 15, 0, B_INFINITY}; // index 16: historically(...) (op: 15)


        // The vector of all nodes
        std::vector<Node> nodes{
            q, p, s, r,       // 0, 1, 2, 3
            not_q,            // 4
            once_q,           // 5
            and_A1,           // 6
            and_A2,           // 7
            once_p,           // 8
            implies_D,        // 9
            not_s,            // 10
            since_F,          // 11
            not_F,            // 12
            and_C,            // 13
            since_B,          // 14
            implies_main,     // 15
            root_always       // 16
        };

        // --- 3. RUN EVALUATION ---
        // Updated file path for RespondBQR dataset
        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/RespondBQR/" + CONDENSATION + "/1M/RespondBQR" + std::to_string(TIMINGS) +".jsonl");
        
        meter.measure([&] { 
            // Rewind the file for every benchmark run
            input_file.clear();
            input_file.seekg(0);
            
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;
            int maxBufferIndex = 0;
            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    // Pass the proposition vector from the *previous* step
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;

                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}





TEST_CASE("Dense RespondGLB", "[dense_benchmarks][RespondGLB]") {

    using namespace db_interval_set;
    using namespace dense_vector;


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

    BENCHMARK_ADVANCED("RespondGLB " + benchmarkName)(Catch::Benchmark::Chronometer meter) {
        IntervalSetHolder holder = newHolder(100000);

        // --- 2. DEFINE FORMULA NODES ---
        // Formula: historically(C)
        // C = (D and E)
        // D = ({s} -> once[a:b] {p})
        // E = not(F)
        // F = (not({s}) since[a:] {p})
        
        // Scale intervals based on TIMINGS
        const int once_a = 3 * (TIMINGS / 10);
        const int once_b = TIMINGS;
        const int since_a = TIMINGS;

        // --- NODES (in topological order) ---
        // Propositions (must be first, matching JSON input order)
        // We assume input is [p, s]
        Node p{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 0
        Node s{empty(holder), empty(holder), NodeType::PROPOSITION, 0, 0, 0, 0};      // index 1

        // Sub-formula D
        Node once_p{empty(holder), empty(holder), NodeType::EVENTUALLY, 0, 0, once_a, once_b}; // index 2: once[a:b] {p} (op: 0)
        Node implies_D{empty(holder), empty(holder), NodeType::IMPLIES, 1, 2, 0, 0};     // index 3: {s} -> once_p (ops: 1, 2)

        // Sub-formula F
        Node not_s{empty(holder), empty(holder), NodeType::NOT, 0, 1, 0, 0};          // index 4: not({s}) (op: 1)
        Node since_F{empty(holder), empty(holder), NodeType::SINCE, 4, 0, since_a, B_INFINITY}; // index 5: not({s}) since[a:] {p} (ops: 4, 0)

        // Sub-formula E
        Node not_F{empty(holder), empty(holder), NodeType::NOT, 0, 5, 0, 0};         // index 6: not(F) (op: 5)

        // Sub-formula C
        Node and_C{empty(holder), empty(holder), NodeType::AND, 3, 6, 0, 0};         // index 7: (D and E) (ops: 3, 6)
        
        // Root node
        Node root_always{empty(holder), empty(holder), NodeType::ALWAYS, 0, 7, 0, B_INFINITY}; // index 8: historically(C) (op: 7)


        // The vector of all nodes
        std::vector<Node> nodes{
            p, s,           // 0, 1
            once_p,         // 2
            implies_D,      // 3
            not_s,          // 4
            since_F,        // 5
            not_F,          // 6
            and_C,          // 7
            root_always     // 8
        };

        // --- 3. RUN EVALUATION ---
        // Updated file path for RespondGLB dataset
        std::ifstream input_file("/home/arinc/workspace/data-oriented-runtime-verif/fullsuite/RespondGLB/" + CONDENSATION + "/1M/RespondGLB" + std::to_string(TIMINGS) +".jsonl");
        
        meter.measure([&] { 
            // Rewind the file for every benchmark run
            input_file.clear();
            input_file.seekg(0);
            
            json_reader::TimescalesInput prevInput;
            int step = 0;
            IntervalSet finalOutput;

            for (std::string line; std::getline(input_file, line);){
                if (step != 0) {
                    auto newInput = json_reader::read_line(line);
                    // Pass the proposition vector from the *previous* step
                    IntervalSet output = run_evaluation(nodes, holder, prevInput.time, newInput.time, prevInput.propositions);
                    prevInput = newInput;
                    finalOutput = output;
                    swapBuffers(holder);
                }
                else {
                    prevInput = json_reader::read_line(line);
                }
                step++;
            }
            return finalOutput;
        });

    };
}


