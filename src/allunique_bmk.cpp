#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <boost/sort/spreadsort/spreadsort.hpp>
#include <iostream>
#include <random>
#include <vector>

#include "radix_sort_hybrid.h"
#include "radix_sort_lsd.h"
#include "radix_sort_msd.h"

#define TEST_SIZE DenseRange(10000, 600000, 50000)

#define BMKBODY(SORT)                                                \
    {                                                                \
        const auto n = state.range(0);                               \
        std::vector<T> values(m_vals.size());                        \
        for (auto _ : state) {                                       \
            std::copy(m_vals.begin(), m_vals.end(), values.begin()); \
            SORT(values.begin(), values.end());                      \
            benchmark::DoNotOptimize(values);                        \
            benchmark::ClobberMemory();                              \
        }                                                            \
    }
using T = uint64_t;

class SortingBmk_allUnique : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);
        std::iota(m_vals.begin(), m_vals.end(), 0);

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(m_vals.begin(), m_vals.end(), g);
    }

    void TearDown(const ::benchmark::State& state) { }
};

// We are interested in stable sort because it is currently used
BENCHMARK_DEFINE_F(SortingBmk_allUnique, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_allUnique, StdStableSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allUnique, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_allUnique, BoostSpreadSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allUnique, MSDRadixSort)
(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<uint64_t> values(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radix_sort_msd(values);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_allUnique, MSDRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allUnique, HybridRadixSort)
(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<uint64_t> values(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radix_sort_hybrid(values);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_allUnique, HybridRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allUnique, LSDRadixSort)
(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<uint64_t> values(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radix_sort_lsd_travis(&values[0], values.size());
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_allUnique, LSDRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

/// uniform random numbers in the range [0, 1e9]
///
class SortingBmk_uniform_1B : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);

        std::default_random_engine generator;
        std::uniform_int_distribution<T> distribution(0, 1e9);

        for (int i = 0; i < n; ++i) {
            m_vals[i] = distribution(generator);
        }
    }

    void TearDown(const ::benchmark::State& state) { }
};

// We are interested in stable sort because it is currently used
BENCHMARK_DEFINE_F(SortingBmk_uniform_1B, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_uniform_1B, StdStableSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_uniform_1B, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_uniform_1B, BoostSpreadSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_uniform_1B, LSDRadixSort)
(benchmark::State& state)
{
    // it is custom due to buffer
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radix_sort_lsd_travis(&values.front(), values.size());
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_uniform_1B, LSDRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_uniform_1B, MSDRadixSort)
(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radix_sort_msd(values);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_uniform_1B, MSDRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_uniform_1B, HybridRadixSort)
(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radix_sort_hybrid(values);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_uniform_1B, HybridRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_MAIN();
