#include <benchmark/benchmark.h>

#include <random>
#include <vector> 
#include <array> 
#include <algorithm>
#include <boost/sort/spreadsort/spreadsort.hpp>

#include "naiveradixsort.h"

#define TEST_SIZE DenseRange(5000, 100000, 5000)

#define BMKBODY(x) \
    { const auto n = state.range(0); \
      std::vector<T> values(m_vals.size());
    for (auto _ : state)
    {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        std::sort(values.begin(), values.end());
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }

class SortingBmk : public benchmark::Fixture
{
public:
    using T = int32_t;
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

    void TearDown(const ::benchmark::State& state) {}
};

BENCHMARK_DEFINE_F(SortingBmk, StdSort)(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    for (auto _ : state)
    {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        std::sort(values.begin(), values.end());
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk, StdSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk, StdStableSort)(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    for (auto _ : state)
    {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        std::stable_sort(values.begin(), values.end());
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk, StdStableSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_DEFINE_F(SortingBmk, BoostSpreadSort)(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    for (auto _ : state)
    {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        boost::sort::spreadsort::spreadsort(values.begin(), values.end());
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk, BoostSpreadSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;


BENCHMARK_DEFINE_F(SortingBmk, NaiveRadixSort)(benchmark::State& state)
{
    const auto n = state.range(0);

    std::vector<T> values(m_vals.size());
    std::vector<T> buffer(m_vals.size());
    for (auto _ : state)
    {
        std::copy(m_vals.begin(), m_vals.end(), values.begin());

        radixSort_count<SortingBmk::T>(values, buffer);
        benchmark::DoNotOptimize(values);
        benchmark::ClobberMemory();
    }
}
BENCHMARK_REGISTER_F(SortingBmk, NaiveRadixSort)->Unit(benchmark::kMicrosecond)->TEST_SIZE;

BENCHMARK_MAIN();
