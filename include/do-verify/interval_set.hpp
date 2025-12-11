#pragma once

#include <algorithm>
#include <vector>
#include <limits>
#include <iostream> // Included for operator<< helper

namespace db_interval_set {

// The 'Interval' struct is just a plain data structure for the API.
// It is NOT the internal storage format.
struct Interval {
    int start;
    int end;
};

// This is the core internal data structure.
// Represents a "half-interval" or a "transition point".
struct Transition {
    int time;
    bool isStart; // true = start of an interval, false = end of an interval
};

struct IntervalSet {
    Transition *buffer;
    int startIndex;
    int endIndex;
};

struct IntervalSetHolder {
    Transition *readBuffer;
    Transition *writeBuffer;
    int writeIndex;
    int bufferSize;
};

// --- THIS IS THE STRUCT YOU PROPOSED ---
// I've renamed it to SegmentIterator as it holds the iterator state.
// I also added the 'domain' field which is necessary to bound the iteration.
struct SegmentIterator {
    // --- Result fields (valid after getNextSegment returns true) ---
    bool leftTruthy;  // The boolean state of leftIntervalSet during the 'interval'
    bool rightTruthy; // The boolean state of rightIntervalSet during the 'interval'
    Interval interval;  // The interval [start, end) for this segment

    // --- Config fields (set at creation) ---
    IntervalSet leftIntervalSet;
    IntervalSet rightIntervalSet;
    Interval domain; // The "world" boundaries for the iteration

    // --- State fields (used internally by the iterator) ---
    int leftIndex;  // The next transition to read from leftIntervalSet
    int rightIndex; // The next transition to read from rightIntervalSet
    
    // --- (FIX) ADDED STATE FIELD ---
    // This holds the start time and state *for the next segment*.
    // The 'result' fields (leftTruthy, rightTruthy, interval) will
    // hold the data for the *completed* segment.
    int currentSegmentStart;
    bool currentLeftTruthy;
    bool currentRightTruthy;
};


inline IntervalSetHolder newHolder(int bufferSize) {
    // Allocate Transition buffers
    return IntervalSetHolder{new Transition[bufferSize], new Transition[bufferSize], 0, bufferSize};
};

inline void swapBuffers(IntervalSetHolder &holder) {
    Transition *temp = holder.readBuffer;
    holder.readBuffer = holder.writeBuffer;
    holder.writeBuffer = temp;
    holder.writeIndex = 0;
};

inline IntervalSet empty(IntervalSetHolder &holder) {
    return IntervalSet{holder.writeBuffer, 1, 0};
};

/**
 * @brief Checks if a single time point is contained within the interval set.
 * Since intervals are [start, end), start is inclusive and end is exclusive.
 *
 * @param set The interval set to check.
 * @param time The time point to query.
 * @return true if the time point is in the set, false otherwise.
 */
inline bool includes(const IntervalSet& set, int time) {
    bool state = false;
    for (int i = set.startIndex; i <= set.endIndex; ++i) {
        const Transition& t = set.buffer[i];
        if (t.time <= time) {
            state = t.isStart;
        } else {
            break; // No need to check further transitions
        }
    }
    return state;
}



/**
 * @brief Creates a new set from a single [start, end) interval.
 * This is the primary way to get data into the system.
 */
inline IntervalSet fromInterval(IntervalSetHolder &holder, Interval interval) {
    int newStartIndex = holder.writeIndex;
    
    // Only add a non-empty interval
    if (interval.start < interval.end) {
        holder.writeBuffer[holder.writeIndex++] = Transition{interval.start, true};
        holder.writeBuffer[holder.writeIndex++] = Transition{interval.end, false};
    }
    
    return IntervalSet{holder.writeBuffer, newStartIndex, holder.writeIndex - 1};
};

/**
 * @brief (NEW) Copies a set from either buffer to the end of the write buffer.
 * This is crucial for carrying over unmodified sets
 * before calling swapBuffers().
 */
inline IntervalSet copySet(IntervalSetHolder& holder, IntervalSet set) {
    int newStartIndex = holder.writeIndex;
    
    // Read from the set's *own* buffer (could be read or write)
    for (int i = set.startIndex; i <= set.endIndex; ++i) {
        // Write to the main writeBuffer
        holder.writeBuffer[holder.writeIndex++] = set.buffer[i];
    }
    
    // Return a new set handle pointing to the new copy
    return IntervalSet{holder.writeBuffer, newStartIndex, holder.writeIndex - 1};
}

/**
 * @brief Computes the union (OR) of two sets using a plane-sweep algorithm.
 */
inline IntervalSet unionSets(IntervalSetHolder &holder, IntervalSet setA, IntervalSet setB) {
    int newStartIndex = holder.writeIndex;
    int i = setA.startIndex;
    int j = setB.startIndex;

    int overlap = 0; // "Winding number" or overlap count (can be 0, 1, or 2)

    while (i <= setA.endIndex || j <= setB.endIndex) {
        // Determine the next time point to process
        int t = std::numeric_limits<int>::max();
        if (i <= setA.endIndex) t = std::min(t, setA.buffer[i].time);
        if (j <= setB.endIndex) t = std::min(t, setB.buffer[j].time);
        
        bool wasInSet = (overlap > 0);

        if (i <= setA.endIndex && setA.buffer[i].time == t) {
            overlap += (setA.buffer[i].isStart ? 1 : -1);
            i++;
        }
        if (j <= setB.endIndex && setB.buffer[j].time == t) {
            overlap += (setB.buffer[j].isStart ? 1 : -1);
            j++;
        }

        bool isInSet = (overlap > 0);

        // If the state changed (e.g., from 0 to 1, or 1 to 0),
        // we write a new transition to the output.
        if (wasInSet != isInSet) {
            holder.writeBuffer[holder.writeIndex++] = Transition{t, isInSet};
        }
    }
    
    return IntervalSet{holder.writeBuffer, newStartIndex, holder.writeIndex - 1};
};

/**
 * @brief Computes the intersection (AND) of two sets.
 */
inline IntervalSet intersectSets(IntervalSetHolder &holder, IntervalSet setA, IntervalSet setB) {
    int newStartIndex = holder.writeIndex;
    int i = setA.startIndex;
    int j = setB.startIndex;

    // A normalized set's "overlap" is just 0 or 1 (i.e., true/false).
    bool isInA = false; 
    bool isInB = false;

    while (i <= setA.endIndex || j <= setB.endIndex) {
        int t = std::numeric_limits<int>::max();
        if (i <= setA.endIndex) t = std::min(t, setA.buffer[i].time);
        if (j <= setB.endIndex) t = std::min(t, setB.buffer[j].time);

        bool wasInSet = (isInA && isInB);

        if (i <= setA.endIndex && setA.buffer[i].time == t) {
            isInA = setA.buffer[i].isStart; // Just update the flag
            i++;
        }
        if (j <= setB.endIndex && setB.buffer[j].time == t) {
            isInB = setB.buffer[j].isStart;
            j++;
        }

        bool isInSet = (isInA && isInB);

        // If the intersection state changed, write a transition
        if (wasInSet != isInSet) {
            holder.writeBuffer[holder.writeIndex++] = Transition{t, isInSet};
        }
    }
    
    return IntervalSet{holder.writeBuffer, newStartIndex, holder.writeIndex - 1};
};

/**
 * @brief Computes the negation of a set within a given domain.
 * This is (domain AND (NOT setA)).
 */
inline IntervalSet negateSet(IntervalSetHolder &holder, IntervalSet setA, Interval domain) {
    int newStartIndex = holder.writeIndex;
    int i = setA.startIndex;

    // We assume setA is "off" (-infinity)
    bool isSetAOn = false;

    // First, process all transitions in setA *before* the domain starts
    // to determine the state of setA right at domain.start
    while (i <= setA.endIndex && setA.buffer[i].time < domain.start) {
        isSetAOn = setA.buffer[i].isStart;
        i++;
    }

    // The negation is the opposite of setA's state
    bool isNegationOn = !isSetAOn;

    // If the negation is "on" at the start of the domain,
    // we must create a 'start' transition.
    if (isNegationOn) {
        holder.writeBuffer[holder.writeIndex++] = Transition{domain.start, true};
    }

    // Now, process all transitions *within* the domain
    while (i <= setA.endIndex && setA.buffer[i].time < domain.end) {
        Transition t = setA.buffer[i];
        
        // The state of setA is changing...
        isSetAOn = t.isStart;
        
        // ...which means the state of the negation is also changing.
        isNegationOn = !isSetAOn;
        
        // Write the corresponding transition for the negated set.
        holder.writeBuffer[holder.writeIndex++] = Transition{t.time, isNegationOn};
        
        i++;
    }

    // We have passed all transitions in setA *before* domain.end.
    // If the negation is still "on", we must close it at domain.end.
    if (isNegationOn) {
        holder.writeBuffer[holder.writeIndex++] = Transition{domain.end, false};
    }
    
    return IntervalSet{holder.writeBuffer, newStartIndex, holder.writeIndex - 1};
};


inline void destroyHolder(IntervalSetHolder &holder) {
    delete[] holder.writeBuffer;
    delete[] holder.readBuffer;
};

// --- NEW SEGMENT ITERATOR FUNCTIONS ---

/**
 * @brief Creates and initializes a SegmentIterator.
 * This iterator walks through a 'domain' and reports the boolean state
 * of setA and setB for each sub-segment.
 *
 * @param setA The "left" interval set.
 * @param setB The "right" interval set.
 * @param domain The interval [start, end) to iterate over.
 * @return An initialized SegmentIterator.
 */
inline SegmentIterator createSegmentIterator(IntervalSet setA, IntervalSet setB, Interval domain) {
    SegmentIterator it;
    it.leftIntervalSet = setA;
    it.rightIntervalSet = setB;
    it.domain = domain;

    int i = setA.startIndex;
    int j = setB.startIndex;

    // 1. Find the state of A and B right *at* domain.start
    // We do this by processing all transitions *before* domain.start
    bool isInA = false;
    while (i <= setA.endIndex && setA.buffer[i].time < domain.start) {
        isInA = setA.buffer[i].isStart;
        i++;
    }

    bool isInB = false;
    while (j <= setB.endIndex && setB.buffer[j].time < domain.start) {
        isInB = setB.buffer[j].isStart;
        j++;
    }
    
    // 2. Set the *initial* internal state for the iterator
    it.leftIndex = i;  // The next index to read from setA
    it.rightIndex = j; // The next index to read from setB
    
    // 3. (FIX) Set the *internal state* for the *first* segment
    // These fields track the segment we are *about* to process.
    it.currentSegmentStart = domain.start;
    it.currentLeftTruthy = isInA;
    it.currentRightTruthy = isInB;

    // The 'result' fields (it.interval, it.leftTruthy, it.rightTruthy)
    // are not yet valid. They are set by the first call to getNextSegment.
    it.interval = {0, 0};
    it.leftTruthy = false;
    it.rightTruthy = false;
    
    return it;
}

/**
 * @brief Advances the iterator to the next segment.
 * @param it The iterator (passed by reference) to advance.
 * @return true if a valid segment was found, false if the iteration is finished.
 */
inline bool getNextSegment(SegmentIterator& it) {
    // Check if the *start* of the segment we are *about* to process
    // is already at or past the domain end.
    if (it.currentSegmentStart >= it.domain.end) {
        return false;
    }

    // 1. Find the time `t` of the *next* transition from either set
    int t = it.domain.end; // Default to the end of the domain

    // Check for the next transition in setA
    if (it.leftIndex <= it.leftIntervalSet.endIndex) {
        int tA = it.leftIntervalSet.buffer[it.leftIndex].time;
        // Only consider transitions *within* the domain
        if (tA < it.domain.end) {
            t = std::min(t, tA);
        }
    }
    // Check for the next transition in setB
    if (it.rightIndex <= it.rightIntervalSet.endIndex) {
        int tB = it.rightIntervalSet.buffer[it.rightIndex].time;
        // Only consider transitions *within* the domain
        if (tB < it.domain.end) {
            t = std::min(t, tB);
        }
    }
    
    // 2. We found the segment.
    // Set the "result" fields to the segment we just *completed*.
    // The state for this segment was stored in the 'current...' fields.
    it.interval.start = it.currentSegmentStart;
    it.interval.end = t;
    it.leftTruthy = it.currentLeftTruthy;
    it.rightTruthy = it.currentRightTruthy;
    
    // 3. Update the *internal state* for the *next* segment (which starts at time `t`)
    bool nextA = it.currentLeftTruthy;
    bool nextB = it.currentRightTruthy;

    // Process all transitions *at* exactly time `t`
    while (it.leftIndex <= it.leftIntervalSet.endIndex &&
           it.leftIntervalSet.buffer[it.leftIndex].time == t) {
        nextA = it.leftIntervalSet.buffer[it.leftIndex].isStart;
        it.leftIndex++;
    }
    
    while (it.rightIndex <= it.rightIntervalSet.endIndex &&
           it.rightIntervalSet.buffer[it.rightIndex].time == t) {
        nextB = it.rightIntervalSet.buffer[it.rightIndex].isStart;
        it.rightIndex++;
    }

    // 4. Store the state for the *next* call
    it.currentSegmentStart = t;   // The *start* of the next segment
    it.currentLeftTruthy = nextA;  // The *state* during the next segment
    it.currentRightTruthy = nextB; // The *state* during the next segment
    
    // We successfully produced a segment.
    return true;
}


// --- Helper Functions ---

/**
 * @brief Converts an IntervalSet (of transitions) back to a
 * std::vector<Interval> for inspection.
 */
inline std::vector<Interval> toVectorIntervals(const IntervalSet& set) {
    std::vector<Interval> result;
    if (set.startIndex > set.endIndex) {
        return result; // Empty set
    }

    int intervalStart = 0;
    // This assumes the set is normalized (no overlapping starts)
    // which our union/intersect/negate functions guarantee.
    
    for (int i = set.startIndex; i <= set.endIndex; ++i) {
        const Transition& t = set.buffer[i];
        if (t.isStart) {
            intervalStart = t.time;
        } else {
            // Only add if it's a valid interval
            if (t.time > intervalStart) {
                 result.push_back(Interval{intervalStart, t.time});
            }
        }
    }
    return result;
}

/**
 * @brief Converts an IntervalSet to a std::vector of its raw transitions.
 */
inline std::vector<Transition> toVectorTransitions(const IntervalSet& set) {
    if (set.startIndex > set.endIndex) {
        return {}; // Return an empty vector
    }
    return std::vector<Transition>(
        set.buffer + set.startIndex, 
        set.buffer + set.endIndex + 1
    );
}

// Helper for std::sort
inline bool compareTransitions(const Transition& a, const Transition& b) {
    if (a.time != b.time) {
        return a.time < b.time;
    }
    // Handle {10, false}, {10, true} case.
    // 'start' (true) should come before 'end' (false).
    return a.isStart > b.isStart;
}

inline IntervalSet createSetFromIntervals(
    IntervalSetHolder& holder, 
    const std::vector<Interval>& intervals) {
    int newStartIndex = holder.writeIndex;
    
    // 1. Create all transitions in a temporary vector
    std::vector<Transition> transitions;
    transitions.reserve(intervals.size() * 2);
    for (const auto& iv : intervals) {
        if (iv.start < iv.end) {
            transitions.push_back({iv.start, true});
            transitions.push_back({iv.end, false});
        }
    }

    // 2. Sort the transitions (O(N log N))
    std::sort(transitions.begin(), transitions.end(), compareTransitions);

    // 3. Perform the union/normalization sweep (O(N))
    int overlap = 0;
    for (const auto& t : transitions) {
        bool wasInSet = (overlap > 0);
        overlap += (t.isStart ? 1 : -1);
        bool isInSet = (overlap > 0);

        if (wasInSet != isInSet) {
            // Check for buffer overflow
            // (In a real benchmark, just make the buffer huge)
            // if (holder.writeIndex >= MAX_BUFFER_SIZE) { ... }
            holder.writeBuffer[holder.writeIndex++] = {t.time, isInSet};
        }
    }
    
    return IntervalSet{holder.writeBuffer, newStartIndex, holder.writeIndex - 1};
}

/**
 * @brief Overload operator== for Interval so Catch2 can compare vectors of them.
 */
inline bool operator==(const Interval& a, const Interval& b) {
    return a.start == b.start && a.end == b.end;
}

/**
 * @brief Overload operator== for Transition for testing.
 */
inline bool operator==(const Transition& a, const Transition& b) {
    return a.time == b.time && a.isStart == b.isStart;
}

/**
 * @brief Overload operator<< for Interval for printing.
 */
inline std::ostream& operator<<(std::ostream& os, const Interval& iv) {
    os << "[" << iv.start << ", " << iv.end << ")";
    return os;
}

/**
 * @brief Overload operator<< for Transition for printing.
 */
inline std::ostream& operator<<(std::ostream& os, const Transition& t) {
    os << "{" << t.time << ", " << (t.isStart ? "T" : "F") << "}";
    return os;
}

/**
 * @brief Overload operator<< for SegmentIterator for printing.
 */
inline std::ostream& operator<<(std::ostream& os, const SegmentIterator& it) {
    os << "Seg{ " << it.interval << ", A:" << (it.leftTruthy ? "T" : "F")
       << ", B:" << (it.rightTruthy ? "T" : "F") << " }";
    return os;
}


} // namespace db_interval_set
