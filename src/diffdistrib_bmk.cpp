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

#define TEST_SIZE Arg(400000)
#define TIME_UNIT Unit(benchmark::kMillisecond)

using T = uint64_t;

class SortingBmk_shuffled : public benchmark::Fixture {
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
BENCHMARK_DEFINE_F(SortingBmk_shuffled, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_shuffled, StdStableSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_shuffled, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_shuffled, BoostSpreadSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_shuffled, MSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_shuffled, MSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_shuffled, HybridRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_shuffled, HybridRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_shuffled, LSDRadixSort)
(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    for (auto _ : state) {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radix_sort_lsd_travis(&values[0], values.size());
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk_shuffled, LSDRadixSort)->TIME_UNIT->TEST_SIZE;

///
class SortingBmk_allequal : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);

        for (int i = 0; i < n; ++i) {
            m_vals[i] = 1000000007; // a prime number
        }
    }

    void TearDown(const ::benchmark::State& state) { }
};

// We are interested in stable sort because it is currently used
BENCHMARK_DEFINE_F(SortingBmk_allequal, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_allequal, StdStableSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allequal, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_allequal, BoostSpreadSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allequal, LSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_allequal, LSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allequal, MSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_allequal, MSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_allequal, HybridRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_allequal, HybridRadixSort)->TIME_UNIT->TEST_SIZE;

///
class SortingBmk_ascending : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);

        for (int i = 0; i < n; ++i) {
            m_vals[i] = i;
        }
    }

    void TearDown(const ::benchmark::State& state) { }
};

// We are interested in stable sort because it is currently used
BENCHMARK_DEFINE_F(SortingBmk_ascending, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_ascending, StdStableSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_ascending, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_ascending, BoostSpreadSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_ascending, LSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_ascending, LSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_ascending, MSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_ascending, MSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_ascending, HybridRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_ascending, HybridRadixSort)->TIME_UNIT->TEST_SIZE;

///
class SortingBmk_descending : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);

        for (int i = 0; i < n; ++i) {
            m_vals[i] = (int)n - i;
        }
    }

    void TearDown(const ::benchmark::State& state) { }
};

BENCHMARK_DEFINE_F(SortingBmk_descending, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_descending, StdStableSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_descending, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_descending, BoostSpreadSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_descending, LSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_descending, LSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_descending, MSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_descending, MSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_descending, HybridRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_descending, HybridRadixSort)->TIME_UNIT->TEST_SIZE;

/// 5 unique elements
///
class SortingBmk_fewunique : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);

        std::default_random_engine generator;
        std::uniform_int_distribution<int> distribution(0, 5);

        T fewUnique[5];
        std::uniform_int_distribution<T> distributionVal(0, std::numeric_limits<T>::max());
        for (int j = 0; j < 5; ++j) {
            fewUnique[j] = distributionVal(generator);
        }

        for (int i = 0; i < n; ++i) {
            int j = distribution(generator);
            m_vals[i] = fewUnique[j];
        }
    }

    void TearDown(const ::benchmark::State& state) { }
};

BENCHMARK_DEFINE_F(SortingBmk_fewunique, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_fewunique, StdStableSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_fewunique, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_fewunique, BoostSpreadSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_fewunique, LSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_fewunique, LSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_fewunique, MSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_fewunique, MSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_fewunique, HybridRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_fewunique, HybridRadixSort)->TIME_UNIT->TEST_SIZE;

/// sorted array with n/1000 swaped elements
///
class SortingBmk_almostsorted : public benchmark::Fixture {
public:
    std::vector<T> m_vals;

    void SetUp(const ::benchmark::State& state)
    {
        const auto n = state.range(0);
        m_vals.resize(n);

        std::default_random_engine generator;
        std::uniform_int_distribution<T> distributionVal(0, std::numeric_limits<T>::max());
        for (int i = 0; i < n; ++i) {
            m_vals[i] = distributionVal(generator);
        }
        std::sort(m_vals.begin(), m_vals.end());

        std::uniform_int_distribution<int> distributionInd(0, n);
        for (int i = 0; i < n / 1000; ++i) {
            int j = distributionInd(generator);
            std::swap(m_vals[2], m_vals[j]);
        }
    }

    void TearDown(const ::benchmark::State& state) { }
};

BENCHMARK_DEFINE_F(SortingBmk_almostsorted, StdStableSort)
(benchmark::State& state)
{
    BMKBODY(std::stable_sort);
}
BENCHMARK_REGISTER_F(SortingBmk_almostsorted, StdStableSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_almostsorted, BoostSpreadSort)
(benchmark::State& state)
{
    BMKBODY(boost::sort::spreadsort::spreadsort);
}
BENCHMARK_REGISTER_F(SortingBmk_almostsorted, BoostSpreadSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_almostsorted, LSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_almostsorted, LSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_almostsorted, MSDRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_almostsorted, MSDRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk_almostsorted, HybridRadixSort)
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
BENCHMARK_REGISTER_F(SortingBmk_almostsorted, HybridRadixSort)->TIME_UNIT->TEST_SIZE;

BENCHMARK_MAIN();
