// Radix sort is taken from https://github.com/travisdowns/sort-bench
/*
MIT License

Copyright (c) 2019 Travis Downs

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <algorithm>
#include <array>
#include <assert.h>
#include <memory>
#include <stack>
#include <string.h>
#include <tuple>

#include <x86intrin.h>

/* HEDLEY_INLINE */
#define HEDLEY_INLINE inline
/* HEDLEY_NEVER_INLINE */
#define HEDLEY_NEVER_INLINE __attribute__((__noinline__))

const size_t RADIX_BITS = 8;
const size_t RADIX_SIZE = (size_t)1 << RADIX_BITS;
const size_t RADIX_LEVELS = (63 / RADIX_BITS) + 1;

using freq_array_type = size_t[RADIX_LEVELS][RADIX_SIZE];

// never inline just to make it show up easily in profiles (inlining this lengthly function doesn't
// really help anyways)
template <class T>
HEDLEY_NEVER_INLINE static void count_frequency(T* a, size_t count, freq_array_type freqs)
{
    constexpr T RADIX_MASK = RADIX_SIZE - 1;
    for (size_t i = 0; i < count; i++) {
        T value = a[i];
        for (size_t pass = 0; pass < RADIX_LEVELS; pass++) {
            freqs[pass][value & RADIX_MASK]++;
            value >>= RADIX_BITS;
        }
    }
}

/**
 * Determine if the frequencies for a given level are "trivial".
 * 
 * Frequencies are trivial if only a single frequency has non-zero
 * occurrences. In that case, the radix step just acts as a copy so we can
 * skip it.
 */
static bool is_trivial(size_t freqs[RADIX_SIZE], size_t count)
{
    for (size_t i = 0; i < RADIX_SIZE; i++) {
        auto freq = freqs[i];
        if (freq != 0) {
            return freq == count;
        }
    }
    assert(count == 0); // we only get here if count was zero
    return true;
}

template <class T>
inline void radix_sort_lsd_travis(T* a, size_t count)
{
    constexpr T RADIX_MASK = RADIX_SIZE - 1;
    std::unique_ptr<T[]> queue_area(new T[count]);

    freq_array_type freqs = {};
    count_frequency(a, count, freqs);

    T *from = a, *to = queue_area.get();

    for (size_t pass = 0; pass < RADIX_LEVELS; pass++) {

        if (is_trivial(freqs[pass], count)) {
            // this pass would do nothing, just skip it
            continue;
        }

        T shift = pass * RADIX_BITS;

        // array of pointers to the current position in each queue, which we set up based on the
        // known final sizes of each queue (i.e., "tighly packed")
        T *queue_ptrs[RADIX_SIZE], *next = to;
        for (size_t i = 0; i < RADIX_SIZE; i++) {
            queue_ptrs[i] = next;
            next += freqs[pass][i];
        }

        // copy each element into the appropriate queue based on the current RADIX_BITS sized
        // "digit" within it
        for (size_t i = 0; i < count; i++) {
            size_t value = from[i];
            size_t index = (value >> shift) & RADIX_MASK;
            *queue_ptrs[index]++ = value;
            // I don't see that big impact
            // __builtin_prefetch(queue_ptrs[index] + 1);
        }

        // swap from and to areas
        std::swap(from, to);
    }

    // because of the last swap, the "from" area has the sorted payload: if it's
    // not the original array "a", do a final copy
    if (from != a) {
        std::copy(from, from + count, a);
    }
}
