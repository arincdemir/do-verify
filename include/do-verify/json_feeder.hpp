#pragma once

#include <string>
#include <vector>
#include <utility>

#include "do-verify/MTLEngine.hpp"
#include "do-verify/interval_set.hpp"

namespace do_verify {

struct DenseJsonFeeder;
struct DiscreteJsonFeeder;

// --- Dense feeder ---

// Create a feeder that streams NDJSON from the given file path.
// The monitor must already have formulas parsed and be finalized.
// Returns nullptr on failure (file not found, parse error, etc.)
DenseJsonFeeder *create_dense_json_feeder(DenseMultiPropertyMonitor &monitor, const std::string &file_path);

// Advance to the next timestep. Populates `output` with evaluation results.
// Returns true if a result was produced, false when the stream is exhausted.
bool feed_next(DenseJsonFeeder *feeder, std::vector<db_interval_set::IntervalSet> &output);

// Returns the start time of the most recently evaluated interval.
int feeder_start_time(const DenseJsonFeeder *feeder);

// Returns the end time of the most recently evaluated interval.
int feeder_end_time(const DenseJsonFeeder *feeder);

// Free all resources associated with the feeder.
void destroy_feeder(DenseJsonFeeder *feeder);

// --- Discrete feeder ---

// Create a feeder that streams NDJSON from the given file path.
// The monitor must already have formulas parsed and be finalized.
// Returns nullptr on failure (file not found, parse error, etc.)
DiscreteJsonFeeder *create_discrete_json_feeder(DiscreteMultiPropertyMonitor &monitor, const std::string &file_path);

// Advance to the next timestep. Populates `output` with evaluation results.
// Returns true if a result was produced, false when the stream is exhausted.
bool feed_next(DiscreteJsonFeeder *feeder, std::vector<bool> &output);

// Returns the time of the most recently evaluated row.
int feeder_time(const DiscreteJsonFeeder *feeder);

// Free all resources associated with the feeder.
void destroy_feeder(DiscreteJsonFeeder *feeder);

} // namespace do_verify
