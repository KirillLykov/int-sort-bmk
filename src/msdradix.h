// Radix sort is taken from https://github.com/travisdowns/sort-bench
// Unknown license

#include <algorithm>
#include <memory>
#include <array>
#include <assert.h>
#include <string.h>
#include <assert.h>
#include <stack>
#include <tuple>

namespace msd_impl
{
using T = uint64_t;

const size_t    RADIX_BITS   = 8;
const size_t    RADIX_SIZE   = (size_t)1 << RADIX_BITS;
const size_t    RADIX_LEVELS = (63 / RADIX_BITS) + 1;
const T  RADIX_MASK   = RADIX_SIZE - 1;

const size_t    DO_FINAL_SWAP = RADIX_LEVELS & 1; // if RADIX_LEVELS is even, buf contains the result

constexpr auto partFunc = [](T v, size_t i) -> int { return (v >> i) & RADIX_MASK; }; // it looks to be inlined

// pass <- [7, 0]
void radix_msd_rec(std::vector<T>& from, std::vector<T>& to, size_t lo, size_t hi, size_t pass) {
    //cout << "ITER " << lo << " " << hi << ", " << pass << endl;
    size_t shift = pass * RADIX_BITS;

    size_t freq[RADIX_SIZE] = {};
    for (size_t i = lo; i < hi; ++i) {
        T value = from[i];
        ++freq[partFunc(value, shift)];
    }

    T* queue_ptrs[RADIX_SIZE];
    queue_ptrs[0] = &to[lo];
    for (size_t i = 1; i < RADIX_SIZE; ++i)
        queue_ptrs[i] = queue_ptrs[i-1] + freq[i-1];

    for (size_t i = lo; i < hi; ++i) {
        T value = from[i];
        size_t index = partFunc(value, shift);
        *queue_ptrs[index] = value;
        ++queue_ptrs[index];
    }

    auto newLo = lo;
    auto newHi = lo + freq[0];
    for (size_t i = 1; i < RADIX_SIZE; ++i) {
        if (newHi - newLo > 1 && pass != 0) // at least one element to sort
            radix_msd_rec(to, from, newLo, newHi, pass - 1);

        newLo = newHi;
        newHi += freq[i];
    }
    if (newHi - newLo > 1 && pass != 0)
        radix_msd_rec(to, from, newLo, newHi, pass - 1);
}
}

void radix_sort_msd(std::vector<T>& data)
{
    std::vector<T> buf(data.size());
    msd_impl::radix_msd_rec(data, buf,  0, data.size(), RADIX_LEVELS-1);
    if (msd_impl::DO_FINAL_SWAP)
        swap(data, buf);
}

