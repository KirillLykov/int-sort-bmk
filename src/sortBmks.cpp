#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <boost/sort/spreadsort/spreadsort.hpp>
#include <iostream>
#include <random>
#include <vector>

#include "boost_spread_sort.h"
#include "naiveradixsort.h"
#include "lessnaiveradixsort.h"

#define TEST_SIZE DenseRange(10000, 400000, 50000)

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

using T = int64_t;

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

// We are interested in spreadsort because it is radix sort for integers
// and we want to achieve performance of this implementation but with stable version
BENCHMARK_DEFINE_F(SortingBmk_allUnique, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_allUnique, BoostSpreadSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allUnique, NaiveRadixSort)
(benchmark::State& state)
{
    // it is custom due to buffer
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    std::vector<T> buffer(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radixSort_count(values, buffer);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_allUnique, NaiveRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

/// uniform random numbers in the range [0, n]
///
class SortingBmk_random_wholeRange : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);

        std::default_random_engine generator;
        std::uniform_int_distribution<T> distribution(0, n);

        for (int i = 0; i < n; ++i) {
            m_vals[i] = distribution(generator);
        }
    }

    void TearDown(const ::benchmark::State& state) { }
};

// We are interested in stable sort because it is currently used
BENCHMARK_DEFINE_F(SortingBmk_random_wholeRange, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_random_wholeRange, StdStableSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

// We are interested in spreadsort because it is radix sort for integers
// and we want to achieve performance of this implementation but with stable version
BENCHMARK_DEFINE_F(SortingBmk_random_wholeRange, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_random_wholeRange, BoostSpreadSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_random_wholeRange, MyIntegerSort)
(benchmark::State& state)
{
    BMKBODY(integer_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_random_wholeRange, MyIntegerSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_random_wholeRange, NaiveRadixSort)
(benchmark::State& state)
{
    // it is custom due to buffer
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    std::vector<T> buffer(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radixSort_count(values, buffer);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_random_wholeRange, NaiveRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_random_wholeRange, LessNaiveRadixSort)
(benchmark::State& state)
{
    // it is custom due to buffer
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radix_sort7((uint64_t*)&values.front(), values.size());
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_random_wholeRange, LessNaiveRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

/// uniform random numbers in the range [0, n/4] (1/4 are unique)
///
class SortingBmk_random_1d4Range : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);

        std::default_random_engine generator;
        std::uniform_int_distribution<T> distribution(0, n / 4);

        for (int i = 0; i < n; ++i) {
            m_vals[i] = distribution(generator);
        }
    }

    void TearDown(const ::benchmark::State& state) { }
};

// We are interested in stable sort because it is currently used
BENCHMARK_DEFINE_F(SortingBmk_random_1d4Range, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_random_1d4Range, StdStableSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

// We are interested in spreadsort because it is radix sort for integers
// and we want to achieve performance of this implementation but with stable version
BENCHMARK_DEFINE_F(SortingBmk_random_1d4Range, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_random_1d4Range, BoostSpreadSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_random_1d4Range, NaiveRadixSort)
(benchmark::State& state)
{
    // it is custom due to buffer
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    std::vector<T> buffer(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radixSort_count(values, buffer);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_random_1d4Range, NaiveRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_MAIN();
