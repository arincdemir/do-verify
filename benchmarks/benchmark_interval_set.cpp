#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_all.hpp>


#include <vector>
#include <random>
#include <chrono>
#include <tuple> // <-- Added for std::tie


#include <boost/icl/interval_set.hpp>
#include <boost/icl/right_open_interval.hpp> // For [start, end)
#include "do-verify/interval_set.hpp"


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


TEST_CASE("Performance Benchmarks", "[interval_set][basic]") {
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


/**
 * @brief Generates a vector of random intervals.
 * (FIXED: Takes generator by reference, does not re-seed)
 */
std::vector<Interval> generateRandomIntervals(
    std::mt19937& gen, // Pass generator by reference
    int count, 
    int maxStart, 
    int maxDuration
) {
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


void run_scenario_serial_update(
    std::vector<IntervalSet>& sets, 
    IntervalSetHolder& holder, 
    int steps,
    const std::vector<bool>& ops
) {
    for (int step = 0; step < steps; step++) {
        
        // sets[0] was not modified. We must copy it.
        sets[0] = copySet(holder, sets[0]);

        // Run the serial update chain.
        for (int i = 1; i < sets.size(); i++) {
            // Use the pre-computed operation
            if (ops[i]) {
                sets[i] = unionSets(holder, sets[i - 1], sets[i]);
            } else {
                sets[i] = intersectSets(holder, sets[i - 1], sets[i]);
            }
        }
        swapBuffers(holder);
    }
}

void run_scenario_boost(
    std::vector<BoostSet>& sets, 
    int steps,
    const std::vector<bool>& ops
) {
    for (int step = 0; step < steps; step++) {
        for (int i = 1; i < sets.size(); i++) {
            // Use the pre-computed operation
            if (ops[i]) {
                sets[i] = sets[i - 1] | sets[i];
            } else {
                sets[i] = sets[i - 1] & sets[i];
            }
        }
    }
}




TEST_CASE("Serial Update Benchmark", "[interval_set][serial]") {

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
        allIntervals[i] = generateRandomIntervals(dataGen, INTERVALS_PER_SET, MAX_START, MAX_DURATION);
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
            run_scenario_serial_update(dbSets, holder, STEPS, operations);
        });

        // Teardown (not timed)
        destroyHolder(holder);
    };
    
    BENCHMARK_ADVANCED("Boost: Serial Update" + benchmark_name) (Catch::Benchmark::Chronometer meter) {
        // This setup code runs *per sample* but is *not timed*.
        std::vector<BoostSet> boostSets(NUM_SETS);
        for (int i = 0; i < NUM_SETS; ++i) {
            boostSets[i] = createBoostSetFromIntervals(allIntervals[i]);
        }

        // This is the code that gets timed
        meter.measure([&] {
            run_scenario_boost(boostSets, STEPS, operations);
        });
        
        // (No teardown needed for Boost, vectors clean up themselves)
    };
}
