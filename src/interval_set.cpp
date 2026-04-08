#include "do-verify/interval_set.hpp"

namespace db_interval_set {

IntervalSetHolder newHolder(int bufferSize) {
    Transition* buffers[2];
    buffers[0] = new Transition[bufferSize];
    buffers[1] = new Transition[bufferSize];
    return IntervalSetHolder{{buffers[0], buffers[1]}, 1, 0, bufferSize};
}

void swapBuffers(IntervalSetHolder &holder) {
    holder.writeBufferIndex = 1 - holder.writeBufferIndex;
    holder.writeIndex = 0;
}

void ensureCapacity(IntervalSetHolder &holder, int requiredSpace) {
    if (holder.writeIndex + requiredSpace > holder.bufferSize) {
        int newSize = std::max(holder.bufferSize * 2, holder.writeIndex + requiredSpace);
        Transition* newBuffer0 = new Transition[newSize];
        Transition* newBuffer1 = new Transition[newSize];
        std::copy(holder.buffers[0], holder.buffers[0] + holder.bufferSize, newBuffer0);
        std::copy(holder.buffers[1], holder.buffers[1] + holder.bufferSize, newBuffer1);
        delete[] holder.buffers[0];
        delete[] holder.buffers[1];
        holder.buffers[0] = newBuffer0;
        holder.buffers[1] = newBuffer1;
        holder.bufferSize = newSize;
    }
}

IntervalSet empty(IntervalSetHolder &holder) {
    return IntervalSet{holder.writeBufferIndex, 1, 0};
}

/**
 * @brief Checks if a single time point is contained within the interval set.
 * Since intervals are [start, end), start is inclusive and end is exclusive.
 *
 * @param set The interval set to check.
 * @param time The time point to query.
 * @return true if the time point is in the set, false otherwise.
 */
bool includes(const IntervalSetHolder& holder, const IntervalSet& set, int time) {
    bool state = false;
    for (int i = set.startIndex; i <= set.endIndex; ++i) {
        const Transition& t = holder.buffers[set.bufferIndex][i];
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
IntervalSet fromInterval(IntervalSetHolder &holder, Interval interval) {
    ensureCapacity(holder, 2);
    int newStartIndex = holder.writeIndex;
    
    // Only add a non-empty interval
    if (interval.start < interval.end) {
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{interval.start, true};
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{interval.end, false};
    }
    
    return IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1};
}

/**
 * @brief (NEW) Copies a set from either buffer to the end of the write buffer.
 * This is crucial for carrying over unmodified sets
 * before calling swapBuffers().
 */
IntervalSet copySet(IntervalSetHolder& holder, IntervalSet set) {
    ensureCapacity(holder, set.endIndex - set.startIndex + 1);
    int newStartIndex = holder.writeIndex;
    
    // Read from the set's *own* buffer (could be read or write)
    for (int i = set.startIndex; i <= set.endIndex; ++i) {
        // Write to the main writeBuffer
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = holder.buffers[set.bufferIndex][i];
    }
    
    // Return a new set handle pointing to the new copy
    return IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1};
}

/**
 * @brief Computes the union (OR) of two sets using a plane-sweep algorithm.
 */
IntervalSet unionSets(IntervalSetHolder &holder, IntervalSet setA, IntervalSet setB) {
    ensureCapacity(holder, (setA.endIndex - setA.startIndex + 1) + (setB.endIndex - setB.startIndex + 1));
    int newStartIndex = holder.writeIndex;
    int i = setA.startIndex;
    int j = setB.startIndex;

    int overlap = 0; // "Winding number" or overlap count (can be 0, 1, or 2)

    while (i <= setA.endIndex || j <= setB.endIndex) {
        // Determine the next time point to process
        int t = std::numeric_limits<int>::max();
        if (i <= setA.endIndex) t = std::min(t, holder.buffers[setA.bufferIndex][i].time);
        if (j <= setB.endIndex) t = std::min(t, holder.buffers[setB.bufferIndex][j].time);
        
        bool wasInSet = (overlap > 0);

        if (i <= setA.endIndex && holder.buffers[setA.bufferIndex][i].time == t) {
            overlap += (holder.buffers[setA.bufferIndex][i].isStart ? 1 : -1);
            i++;
        }
        if (j <= setB.endIndex && holder.buffers[setB.bufferIndex][j].time == t) {
            overlap += (holder.buffers[setB.bufferIndex][j].isStart ? 1 : -1);
            j++;
        }

        bool isInSet = (overlap > 0);

        // If the state changed (e.g., from 0 to 1, or 1 to 0),
        // we write a new transition to the output.
        if (wasInSet != isInSet) {
            holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{t, isInSet};
        }
    }
    
    return IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1};
}


/**
 * @brief Computes the union of a set with an interval, where the interval 
 * is guaranteed to be towards the right, and have at most one overlap.
 */
IntervalSet unionIntervalFromRight(IntervalSetHolder &holder, IntervalSet set, Interval interval) {
    ensureCapacity(holder, (set.endIndex - set.startIndex + 1) + 2);
    // Case 1: Handle Empty Set
    if (set.startIndex > set.endIndex) {
        return fromInterval(holder, interval);
    }
    int newStartIndex = holder.writeIndex;
    // Case 2: Disjoint (No overlap, simply append)
    if (holder.buffers[set.bufferIndex][set.endIndex].time < interval.start) { // cannot merge
        // Copy existing set
        for (int i = set.startIndex; i <= set.endIndex; ++i) {
            holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = holder.buffers[set.bufferIndex][i];
        }
        // Append new interval
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{interval.start, true};
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{interval.end, false};
        return IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1};
    }
    // Case 3: Merge (Overlap or Touching)
    else {
        // Copy everything EXCEPT the very last transition (the old end point)
        for (int i = set.startIndex; i < set.endIndex; ++i) {
            holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = holder.buffers[set.bufferIndex][i];
        }
        // Write the new extended end transition
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{interval.end, false};
        return IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1};
    }
}

/**
 * @brief Computes the intersection (AND) of two sets.
 */
IntervalSet intersectSets(IntervalSetHolder &holder, IntervalSet setA, IntervalSet setB) {
    ensureCapacity(holder, (setA.endIndex - setA.startIndex + 1) + (setB.endIndex - setB.startIndex + 1));
    int newStartIndex = holder.writeIndex;
    int i = setA.startIndex;
    int j = setB.startIndex;

    // A normalized set's "overlap" is just 0 or 1 (i.e., true/false).
    bool isInA = false; 
    bool isInB = false;

    while (i <= setA.endIndex || j <= setB.endIndex) {
        int t = std::numeric_limits<int>::max();
        if (i <= setA.endIndex) t = holder.buffers[setA.bufferIndex][i].time;
        if (j <= setB.endIndex) t = std::min(t, holder.buffers[setB.bufferIndex][j].time);

        bool wasInSet = (isInA && isInB);

        if (i <= setA.endIndex && holder.buffers[setA.bufferIndex][i].time == t) {
            isInA = holder.buffers[setA.bufferIndex][i].isStart; // Just update the flag
            i++;
        }
        if (j <= setB.endIndex && holder.buffers[setB.bufferIndex][j].time == t) {
            isInB = holder.buffers[setB.bufferIndex][j].isStart;
            j++;
        }

        bool isInSet = (isInA && isInB);

        // If the intersection state changed, write a transition
        if (wasInSet != isInSet) {
            holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{t, isInSet};
        }
    }
    
    return IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1};
}

/**
 * @brief Computes the negation of a set within a given domain.
 * This is (domain AND (NOT setA)).
 */
IntervalSet negateSet(IntervalSetHolder &holder, IntervalSet setA, Interval domain) {
    ensureCapacity(holder, (setA.endIndex - setA.startIndex + 1) + 2);
    int newStartIndex = holder.writeIndex;
    int i = setA.startIndex;

    // We assume setA is "off" (-infinity)
    bool isSetAOn = false;

    // First, process all transitions in setA *before* the domain starts
    // to determine the state of setA right at domain.start
    while (i <= setA.endIndex && holder.buffers[setA.bufferIndex][i].time < domain.start) {
        isSetAOn = holder.buffers[setA.bufferIndex][i].isStart;
        i++;
    }

    // The negation is the opposite of setA's state
    bool isNegationOn = !isSetAOn;

    // If the negation is "on" at the start of the domain,
    // we must create a 'start' transition.
    if (isNegationOn) {
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{domain.start, true};
    }

    // Now, process all transitions *within* the domain
    while (i <= setA.endIndex && holder.buffers[setA.bufferIndex][i].time < domain.end) {
        Transition t = holder.buffers[setA.bufferIndex][i];
        
        // The state of setA is changing...
        isSetAOn = t.isStart;
        
        // ...which means the state of the negation is also changing.
        isNegationOn = !isSetAOn;
        
        // Write the corresponding transition for the negated set.
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{t.time, isNegationOn};
        
        i++;
    }

    // We have passed all transitions in setA *before* domain.end.
    // If the negation is still "on", we must close it at domain.end.
    if (isNegationOn) {
        holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{domain.end, false};
    }
    
    return IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1};
}

void destroyHolder(IntervalSetHolder &holder) {
    delete[] holder.buffers[0];
    delete[] holder.buffers[1];
}

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
SegmentIterator createSegmentIterator(const IntervalSetHolder& holder, IntervalSet setA, IntervalSet setB, Interval domain) {
    SegmentIterator it;
    it.holder = &holder;
    it.leftIntervalSet = setA;
    it.rightIntervalSet = setB;
    it.domain = domain;

    int i = setA.startIndex;
    int j = setB.startIndex;

    // 1. Find the state of A and B right *at* domain.start
    // We do this by processing all transitions *before* domain.start
    bool isInA = false;
    while (i <= setA.endIndex && holder.buffers[setA.bufferIndex][i].time < domain.start) {
        isInA = holder.buffers[setA.bufferIndex][i].isStart;
        i++;
    }

    bool isInB = false;
    while (j <= setB.endIndex && holder.buffers[setB.bufferIndex][j].time < domain.start) {
        isInB = holder.buffers[setB.bufferIndex][j].isStart;
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
bool getNextSegment(SegmentIterator& it) {
    // Check if the *start* of the segment we are *about* to process
    // is already at or past the domain end.
    if (it.currentSegmentStart >= it.domain.end) {
        return false;
    }

    // 1. Find the time `t` of the *next* transition from either set
    int t = it.domain.end; // Default to the end of the domain

    // Check for the next transition in setA
    if (it.leftIndex <= it.leftIntervalSet.endIndex) {
        int tA = it.holder->buffers[it.leftIntervalSet.bufferIndex][it.leftIndex].time;
        // Only consider transitions *within* the domain
        if (tA < it.domain.end) {
            t = std::min(t, tA);
        }
    }
    // Check for the next transition in setB
    if (it.rightIndex <= it.rightIntervalSet.endIndex) {
        int tB = it.holder->buffers[it.rightIntervalSet.bufferIndex][it.rightIndex].time;
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
           it.holder->buffers[it.leftIntervalSet.bufferIndex][it.leftIndex].time == t) {
        nextA = it.holder->buffers[it.leftIntervalSet.bufferIndex][it.leftIndex].isStart;
        it.leftIndex++;
    }
    
    while (it.rightIndex <= it.rightIntervalSet.endIndex &&
           it.holder->buffers[it.rightIntervalSet.bufferIndex][it.rightIndex].time == t) {
        nextB = it.holder->buffers[it.rightIntervalSet.bufferIndex][it.rightIndex].isStart;
        it.rightIndex++;
    }

    // 4. Store the state for the *next* call
    it.currentSegmentStart = t;   // The *start* of the next segment
    it.currentLeftTruthy = nextA;  // The *state* during the next segment
    it.currentRightTruthy = nextB; // The *state* during the next segment
    
    // We successfully produced a segment.
    return true;
}


/**
 * @brief Checks if 'time' is in the set AND returns a new set clipped to [time + 1, INF).
 * Optimized replacement for: includes(holder, t) + intersect(fromInterval(t+1, INF)).
 * * PRECONDITION: The input set must not contain any transitions before 'time'.
 * (i.e., holder.buffers[set.bufferIndex][set.startIndex].time >= time).
 */
CheckAndClipResult checkAndClip(IntervalSetHolder &holder, IntervalSet set, int time) {
    ensureCapacity(holder, (set.endIndex - set.startIndex + 1) + 2);
    // 1. Handle Empty Set
    if (set.startIndex > set.endIndex) {
        return CheckAndClipResult{false, set}; // Set is empty, output is false
    }

    int newStartIndex = holder.writeIndex;
    Transition first = holder.buffers[set.bufferIndex][set.startIndex];

    // Case A: The set starts in the future (> time)
    // Therefore, it does NOT include 'time'.
    if (first.time > time) {
        // Copy the set exactly as is (it is already valid for [time+1, inf))
        for (int i = set.startIndex; i <= set.endIndex; ++i) {
            holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = holder.buffers[set.bufferIndex][i];
        }
        return CheckAndClipResult{false, IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1}};
    }
    
    // Case B: The set starts exactly at 'time'
    // Therefore, it DOES include 'time'.
    else {
        // We need to clip the start. The interval [time, X) becomes [time+1, X).
        
        int readIdx = set.startIndex + 1;
        
        // Check for [time, time+1)
        // If the interval ends exactly at time+1, the result [time+1, time+1) is empty.
        // We can skip writing the new start and skip copying the end.
        bool skipEmptyInterval = false;
        if (readIdx <= set.endIndex) {
            Transition nextT = holder.buffers[set.bufferIndex][readIdx];
            if (nextT.time == (time + 1) && !nextT.isStart) {
                skipEmptyInterval = true;
                readIdx++; // Skip reading the 'end' transition
            }
        }

        if (!skipEmptyInterval) {
            // Write the new start transition at time + 1
            holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = Transition{time + 1, true};
        }
        
        // Copy the rest of the transitions (if any)
        while (readIdx <= set.endIndex) {
            holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = holder.buffers[set.bufferIndex][readIdx];
            readIdx++;
        }

        return CheckAndClipResult{true, IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1}};
    }
}


// --- Helper Functions ---

/**
 * @brief Converts an IntervalSet (of transitions) back to a
 * std::vector<Interval> for inspection.
 */
std::vector<Interval> toVectorIntervals(const IntervalSetHolder& holder, const IntervalSet& set) {
    std::vector<Interval> result;
    if (set.startIndex > set.endIndex) {
        return result; // Empty set
    }

    int intervalStart = 0;
    // This assumes the set is normalized (no overlapping starts)
    // which our union/intersect/negate functions guarantee.
    
    for (int i = set.startIndex; i <= set.endIndex; ++i) {
        const Transition& t = holder.buffers[set.bufferIndex][i];
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
std::vector<Transition> toVectorTransitions(const IntervalSetHolder& holder, const IntervalSet& set) {
    if (set.startIndex > set.endIndex) {
        return {}; // Return an empty vector
    }
    return std::vector<Transition>(
        holder.buffers[set.bufferIndex] + set.startIndex, 
        holder.buffers[set.bufferIndex] + set.endIndex + 1
    );
}

// Helper for std::sort
bool compareTransitions(const Transition& a, const Transition& b) {
    if (a.time != b.time) {
        return a.time < b.time;
    }
    // Handle {10, false}, {10, true} case.
    // 'start' (true) should come before 'end' (false).
    return a.isStart > b.isStart;
}

IntervalSet createSetFromIntervals(
    IntervalSetHolder& holder, 
    const std::vector<Interval>& intervals) {
    ensureCapacity(holder, intervals.size() * 2);
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
            holder.buffers[holder.writeBufferIndex][holder.writeIndex++] = {t.time, isInSet};
        }
    }
    
    return IntervalSet{holder.writeBufferIndex, newStartIndex, holder.writeIndex - 1};
}

/**
 * @brief Overload operator== for Interval so Catch2 can compare vectors of them.
 */
bool operator==(const Interval& a, const Interval& b) {
    return a.start == b.start && a.end == b.end;
}

/**
 * @brief Overload operator== for Transition for testing.
 */
bool operator==(const Transition& a, const Transition& b) {
    return a.time == b.time && a.isStart == b.isStart;
}

/**
 * @brief Overload operator<< for Interval for printing.
 */
std::ostream& operator<<(std::ostream& os, const Interval& iv) {
    os << "[" << iv.start << ", " << iv.end << ")";
    return os;
}

/**
 * @brief Overload operator<< for Transition for printing.
 */
std::ostream& operator<<(std::ostream& os, const Transition& t) {
    os << "{" << t.time << ", " << (t.isStart ? "T" : "F") << "}";
    return os;
}

/**
 * @brief Overload operator<< for SegmentIterator for printing.
 */
std::ostream& operator<<(std::ostream& os, const SegmentIterator& it) {
    os << "Seg{ " << it.interval << ", A:" << (it.leftTruthy ? "T" : "F")
       << ", B:" << (it.rightTruthy ? "T" : "F") << " }";
    return os;
}

} // namespace db_interval_set
