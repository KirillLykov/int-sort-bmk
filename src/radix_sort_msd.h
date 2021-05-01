// Radix sort is taken from https://github.com/travisdowns/sort-bench
// Unknown license

#include <algorithm>
#include <array>
#include <assert.h>
#include <memory>
#include <string.h>

namespace msd_impl {
const size_t RADIX_BITS = 8;
const size_t RADIX_SIZE = 1ull << RADIX_BITS;
const size_t RADIX_LEVELS = (63ull / RADIX_BITS) + 1ull;
const size_t DO_FINAL_SWAP = RADIX_LEVELS & 1ull; // if RADIX_LEVELS is even, buf contains the result

static bool is_trivial(size_t freqs[RADIX_SIZE], size_t count)
{
    for (size_t i = 0; i < RADIX_SIZE; i++) {
        auto freq = freqs[i];
        if (freq != 0) {
            return freq == count;
        }
    }
    return true;
}

// pass <- [7, 0]
template <class T>
void radix_msd_rec(std::vector<T>& from, std::vector<T>& to, size_t lo, size_t hi, size_t pass)
{
    constexpr T RADIX_MASK = RADIX_SIZE - 1;
    auto partFunc = [RADIX_MASK](T v, size_t i) -> T { return (v >> i) & RADIX_MASK; }; // inlined
    if (hi - lo < 16) { // there will be insertion sort under the hood
        auto s = &from.front() + lo;
        auto e = &from.front() + hi;
        std::stable_sort(s, e);
        return;
    }

    size_t shift = pass * RADIX_BITS;

    size_t freq[RADIX_SIZE] = {};
    for (size_t i = lo; i < hi; ++i) {
        T value = from[i];
        ++freq[partFunc(value, shift)];
    }
    if (is_trivial(freq, hi - lo)) {
        if (pass != 0) // at least one element to sort
            radix_msd_rec(from, to, lo, hi, pass - 1);
        return;
    }

    T* queue_ptrs[RADIX_SIZE];
    queue_ptrs[0] = &to[lo];
    for (size_t i = 1; i < RADIX_SIZE; ++i)
        queue_ptrs[i] = queue_ptrs[i - 1] + freq[i - 1];

    for (size_t i = lo; i < hi; ++i) {
        T value = from[i];
        size_t index = partFunc(value, shift);
        *queue_ptrs[index] = value;
        ++queue_ptrs[index];
    }

    auto newLo = lo;
    auto newHi = lo + freq[0];
    for (size_t i = 1; i < RADIX_SIZE; ++i) {
        if (newHi - newLo > 1 && pass != 0) { // at least one element to sort
            radix_msd_rec(to, from, newLo, newHi, pass - 1);
        }
        newLo = newHi;
        newHi += freq[i];
    }
    if (newHi - newLo > 1 && pass != 0)
        radix_msd_rec(to, from, newLo, newHi, pass - 1);
}
}

template <class T>
void radix_sort_msd(std::vector<T>& data)
{
    std::vector<T> buf(data.size());
    msd_impl::radix_msd_rec(data, buf, 0, data.size(), RADIX_LEVELS - 1);
    if (msd_impl::DO_FINAL_SWAP)
        swap(data, buf);
}
