# AGENTS.md: do-verify Project Context

## Project Overview

**do-verify** is a data-oriented runtime verification engine implementing Metric Temporal Logic (MTL) monitoring algorithms. It's based on the paper "Online monitoring of metric temporal logic using sequential networks" (https://arxiv.org/abs/1901.00175).

### Key Characteristics
- **Language**: C++20
- **Build System**: CMake 3.15+
- **Core Type**: Static library (`libdo-verify.a`)
- **Primary Use Case**: Efficient, streaming-based evaluation of temporal logic formulas over sequences of propositions

---

## Repository Structure

```
do-verify/
├── CMakeLists.txt                 # Main build configuration
├── README.md                       # Project documentation
├── include/do-verify/             # Public headers
│   ├── MTLEngine.hpp              # Monitor classes & evaluation interface
│   ├── interval_set.hpp           # Core interval data structure
│   ├── ptl_grammar.hpp            # PEG grammar for formula syntax
│   ├── ptl.hpp                    # Formula parser implementation
│   ├── json_feeder.hpp            # NDJSON streaming interface
│   ├── json_reader.hpp            # JSON row parsing
│   └── binary_row_reader.hpp      # Binary format input
├── src/                           # Implementation
│   ├── main.cpp                   # Binary evaluation tool
│   ├── main_perf.cpp              # Profiling executable
│   ├── test_parse.cpp             # Parser test utility
│   ├── json_read_main.cpp         # JSON test utility
│   ├── MTLEngine.cpp              # Engine implementation
│   ├── interval_set.cpp           # Interval operations
│   ├── json_feeder.cpp            # JSON streaming
│   ├── json_reader.cpp            # Row parsing
│   └── binary_row_reader.cpp      # Binary reading
├── tests/                         # Unit tests (Catch2 framework)
│   ├── CMakeLists.txt
│   ├── test_discrete.cpp          # Discrete time model tests
│   ├── test_dense.cpp             # Dense time model tests
│   ├── test_interval_set.cpp      # Interval operations tests
│   ├── test_parse.cpp             # Parser correctness tests
│   ├── test_readers.cpp           # Input reader tests
│   ├── test_json_input.cpp        # JSON feeder tests
│   └── data/                      # Test data files
├── benchmarks/                    # Performance benchmarks
│   ├── CMakeLists.txt
│   ├── benchmark_engine.cpp       # Engine performance
│   ├── benchmark_interval_set.cpp # Interval operations perf
│   ├── benchmark_multi_property.cpp # Multi-property monitoring
│   └── data/                      # Full benchmark suite
└── build/                         # Build artifacts (generated)
```

---

## Core Architectural Components

### 1. MTLEngine Module (`MTLEngine.hpp`)

**Two Temporal Models:**

- **DiscreteMultiPropertyMonitor**: Point-in-time semantics (evaluates at discrete time points)
  - Output type: `std::vector<bool>`
  - Evaluation: `eval_multi_property(monitor, time, propositions) → bool`
  
- **DenseMultiPropertyMonitor**: Interval-based semantics (evaluates over time intervals)
  - Output type: `std::vector<IntervalSet>`
  - Evaluation: `eval_multi_property(monitor, startTime, endTime, propositions) → IntervalSet`

**Node Types (Operators):**
- `PROPOSITION`: Input proposition (leaf node)
- `AND`, `OR`, `NOT`: Boolean operators
- `IMPLIES`: Implication (→)
- `EVENTUALLY` / `ONCE`: Future temporal operator (P or ◇)
- `ALWAYS` / `HISTORICALLY`: Past temporal operator (H or □)
- `SINCE`: Past temporal operator (S)

**Key Structs:**
- `ParsedNode`: Parsed formula node with type and operand indices
- `DiscreteNode` / `DenseNode`: Runtime node with state
- `DiscreteMultiPropertyMonitor` / `DenseMultiPropertyMonitor`: Container for nodes and state

### 2. IntervalSet Module (`interval_set.hpp`)

**Core Insight**: Temporal intervals are represented as **Transition arrays** (start/end markers) for efficient manipulation.

**Key Structs:**
- `Interval`: [start, end) pair (public API)
- `Transition`: Internal representation (time + isStart flag)
- `IntervalSet`: Buffer-based interval set (transitions + indices)
- `IntervalSetHolder`: Dual-buffer system for swap-based computation
- `SegmentIterator`: Iterator over union of two interval sets

**Key Operations:**
- `fromInterval()`: Create set from single interval
- `includes()`: Check if time point is in set
- `unionIntervalSets()`: Compute union (OR operation)
- `intersectIntervalSets()`: Compute intersection (AND operation)
- `complementIntervalSet()`: Compute complement (NOT operation)
- `sinceIntervalSets()`: Compute SINCE operation
- `swapBuffers()`: Flip read/write buffers for next timestep

**Buffer Management:**
- Dual-buffer system prevents memory reallocation
- Pre-allocated for maximum configuration size
- Must call `swapBuffers()` after each evaluation step

### 3. Formula Parser (`ptl.hpp` & `ptl_grammar.hpp`)

**Grammar** (PEG-based, defined in `ptl_grammar.hpp`):
```
Expression  ← Implicative
Implicative ← Disjunctive (-> Disjunctive)?      # Implication
Disjunctive ← Conjunctive (|| Conjunctive)*      # OR
Conjunctive ← SinceExpr (&& SinceExpr)*          # AND
SinceExpr   ← Unary (S Bound? Unary)?            # SINCE operator
Unary       ← NotExpr | OnceExpr | HistExpr | Atom | (Expression)
NotExpr     ← ! Atom
OnceExpr    ← once[a:b] Atom                     # EVENTUALLY with bounds
HistExpr    ← H[a:b] Atom                        # HISTORICALLY with bounds
Atom        ← {proposition_name}
Bound       ← [a:b] | [a:inf] | [:b]
```

**Supported Syntax:**
- Propositions: `{p}`, `{q}`, etc.
- Boolean: `&&` (and), `||` (or), `!` (not), `->` (implies)
- Temporal (future): `once`, `P` (Eventually/Eventually)
- Temporal (past): `historically`, `H` (Always/Always)
- Since: `since`, `S`
- Time bounds: `[2:5]`, `[2:inf]`, `[:10]`

**Usage:**
```cpp
ptl_parser parser;
DiscreteMultiPropertyMonitor monitor = createDiscreteMultiPropertyMonitor(1000);
parser.parse_discrete("H[1:3]({p} && {q})", monitor);
finalize_monitor(monitor, {"p", "q"});
```

### 4. JSON Feeder (`json_feeder.hpp`)

**Purpose**: Stream NDJSON data row-by-row into a monitor.

**API:**
- `create_dense_json_feeder(monitor, filepath)`: Create Dense-time feeder
- `create_discrete_json_feeder(monitor, filepath)`: Create Discrete-time feeder
- `feed_next(feeder, output_vector)`: Advance to next row, populate output
- `destroy_feeder(feeder)`: Clean up resources

**Expected NDJSON Format:**
```json
{"time": 0, "p": true, "q": false, "r": true}
{"time": 1, "p": false, "q": true, "r": false}
```

### 5. Input Formats

**Binary Row Format** (`binary_row_reader.hpp`):
```cpp
#pragma pack(push, 1)
struct TimescalesInput {
    int32_t time;
    bool p, q, r, s;
};
#pragma pack(pop)
```
- Fixed-size binary records (13 bytes each)
- Used for benchmarking (faster than JSON)

**JSON Format** (`json_reader.hpp`):
- NDJSON (newline-delimited JSON)
- Flexible proposition names
- Typical usage: `{"time": int, "prop1": bool, "prop2": bool, ...}`

---

## Build & Compilation

### Prerequisites
- CMake 3.15+
- C++20 compatible compiler (GCC 10+, Clang 13+)
- Dependencies auto-fetched: simdjson, cpp-peglib, Catch2, Boost (headers)

### Build Commands

```bash
# Create build directory
mkdir -p build && cd build

# Configure (default)
cmake ..

# Configure with coverage
cmake -DENABLE_COVERAGE=ON ..

# Build library and executables
cmake --build .

# Build with verbose output
cmake --build . --verbose

# Parallel build (4 jobs)
cmake --build . -j4
```

### Generated Targets

| Target | Type | Source | Purpose |
|--------|------|--------|---------|
| `libdo-verify.a` | Library | Multiple | Core library |
| `do-verify-bin` | Executable | `src/main.cpp` | Binary file evaluation |
| `do-verify-perf` | Executable | `src/main_perf.cpp` | Performance profiling (-g -fno-omit-frame-pointer) |
| `test-parse` | Executable | `src/test_parse.cpp` | Parser testing (-g -O0) |
| `test-json` | Executable | `src/json_read_main.cpp` | JSON reading test |
| `unit_tests` | Executable | `tests/*.cpp` | Comprehensive test suite |
| `bench_runner` | Executable | `benchmarks/*.cpp` | Benchmark suite (Catch2 + Boost) |

---

## Testing

### Unit Tests (Catch2 Framework)

```bash
cd build
./tests/unit_tests

# Run tests with specific tag
./tests/unit_tests "[discrete]"
./tests/unit_tests "[dense]"
./tests/unit_tests "[AbsentAQ]"

# List all tests
./tests/unit_tests --list-tests

# Generate report
./tests/unit_tests --reporter=xml > report.xml
```

**Test Files:**
- `test_discrete.cpp`: Discrete-time model correctness
- `test_dense.cpp`: Dense-time model correctness
- `test_interval_set.cpp`: Interval operations
- `test_parse.cpp`: Formula parser
- `test_readers.cpp`: Input readers
- `test_json_input.cpp`: JSON feeder

### Benchmarks

```bash
cd build/benchmarks
./bench_runner

# Run specific benchmark
./bench_runner "[benchmark]"
./bench_runner "[engine]"
```

**Benchmark Files:**
- `benchmark_engine.cpp`: Monitor evaluation performance
- `benchmark_interval_set.cpp`: Interval operation speed
- `benchmark_multi_property.cpp`: Multi-property monitoring

### Code Coverage

```bash
cd build
cmake -DENABLE_COVERAGE=ON ..
cmake --build .
make coverage

# Output: build/coverage/index.html
```

---

## Common Development Workflows

### 1. Creating a Monitor & Evaluating a Formula

```cpp
using namespace do_verify;
using namespace db_interval_set;

// Discrete time model
auto monitor = createDiscreteMultiPropertyMonitor(1000);
ptl_parser parser;
parser.parse_discrete("H[1:2]{p} || once[0:5]{q}", monitor);
finalize_monitor(monitor, {"p", "q"});

// Evaluate at time=3
std::vector<bool> inputs = {true, false};
const auto& results = eval_multi_property(monitor, 3, inputs);
```

### 2. Streaming JSON Data

```cpp
auto monitor = createDiscreteMultiPropertyMonitor(1000);
ptl_parser parser;
parser.parse_discrete(formula, monitor);
finalize_monitor(monitor, {"p", "q", "r"});

auto feeder = create_discrete_json_feeder(monitor, "input.ndjson");
std::vector<bool> outputs;
while (feed_next(feeder, outputs)) {
    for (size_t i = 0; i < outputs.size(); ++i) {
        printf("Property %zu: %s\n", i, outputs[i] ? "true" : "false");
    }
}
destroy_feeder(feeder);
```

### 3. Adding a New Operator

1. Add `NodeType` enum value in `MTLEngine.hpp`
2. Implement parser rule in `ptl_grammar.hpp` and `ptl.hpp`
3. Implement discrete evaluation in `MTLEngine.cpp::run_evaluation(DiscreteNode...)`
4. Implement dense evaluation in `MTLEngine.cpp::run_evaluation(DenseNode...)`
5. Add test cases in `tests/test_discrete.cpp` and `tests/test_dense.cpp`

### 4. Optimizing Performance

- Use `-O3` compiler flag for release builds
- Profile with `do-verify-perf` (includes frame pointers for profiling tools)
- Run benchmarks: `build/benchmarks/bench_runner`
- Use binary input format (faster than JSON parsing)

---

## Key Implementation Details

### Buffer Lifecycle (IntervalSetHolder)

```
1. Create: holder = newHolder(bufferSize)
2. For each evaluation:
   a. Compute results (writes to writeBuffer)
   b. Call swapBuffers(holder)
   c. Results now in readBuffer
3. Destroy: destroyHolder(holder)
```

### Node Deduplication

- Parser uses `NodeKeyHash` and `node_dedup_map` to avoid duplicate nodes
- Identical subexpressions share the same node index
- Reduces memory usage and speeds evaluation

### Discrete vs Dense Semantics

| Aspect | Discrete | Dense |
|--------|----------|-------|
| Time Model | Point samples | Intervals |
| Interval Semantics | None | Continuous truth intervals |
| EVENTUALLY | "∃t ∈ [a,b]: φ(t)" | Union of satisfaction intervals |
| ALWAYS | "∀t ∈ [a,b]: φ(t)" | Intersection of satisfaction intervals |
| Output Type | `bool` | `IntervalSet` |

---

## Data & Benchmarks

### Benchmark Suite Location
```
data/fullsuite/{TEST_PATTERN}/{CONDENSATION}/{SIZE}/
```

**Parameters:**
- TEST_PATTERN: AbsentAQ, AbsentBQR, AbsentBR, AlwaysAQ, AlwaysBQR, AlwaysBR, RecurBQR, RecurGLB, RespondBQR, RespondGLB
- CONDENSATION: Dense1, Dense10, Dense100, Discrete
- SIZE: 1M (1 million rows)

**File Format:** Binary (`.row.bin`) or NDJSON

---

## Dependencies

| Dependency | Version | Purpose | Linking |
|------------|---------|---------|---------|
| simdjson | v4.3.0 | Fast JSON parsing | PUBLIC |
| cpp-peglib | v1.9.1 | PEG parser generator | PUBLIC |
| Catch2 | v3.4.0 | Unit testing | Tests only |
| Boost | Latest | ICL (interval container lib) | Benchmarks only |

---

## Troubleshooting

| Issue | Cause | Solution |
|-------|-------|----------|
| "undeclared identifier: ptl_parser" | Missing header | `#include "do-verify/ptl.hpp"` |
| Segfault in buffer operations | Not calling `swapBuffers()` | Call `swapBuffers(holder)` after each step |
| Parser error with formula | Invalid syntax | Check against grammar rules in ptl_grammar.hpp |
| Tests fail with data directory | Data not copied | Run `cmake --build .` in build/ |
| Monitor produces incorrect results | Propositions misaligned | Ensure `finalize_monitor()` proposition order matches input order |
