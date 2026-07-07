// Benchmark.h - Stub for EA/Westwood benchmark system
#pragma once
#ifndef BENCHMARK_H
#define BENCHMARK_H

// Stub benchmark function - does nothing in this build
inline void RunBenchmark(int mode, void* param, float* floatIdx, float* intIdx, float* memIdx) {
    (void)mode; (void)param;
    if (floatIdx) *floatIdx = 1.0f;
    if (intIdx)   *intIdx   = 1.0f;
    if (memIdx)   *memIdx   = 1.0f;
}

#endif // BENCHMARK_H
