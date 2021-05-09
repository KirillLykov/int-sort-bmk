#include <algorithm>
#include <array>
#include <assert.h>
#include <memory>
#include <string.h>

namespace details {
const size_t RADIX_BITS = 8;
const size_t RADIX_SIZE = 1ull << RADIX_BITS;
const size_t RADIX_LEVELS = (63ull / RADIX_BITS) + 1ull;
const size_t DO_FINAL_SWAP = RADIX_LEVELS & 1ull; // if RADIX_LEVELS is even, buf contains the result
const size_t LSD_THRESHOLD = 1 << 14;

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

using freq_array_type = size_t[RADIX_LEVELS][RADIX_SIZE];

template <class T>
static void count_frequency(T* a, size_t count, freq_array_type freqs, size_t hiPass)
{
    constexpr T RADIX_MASK = RADIX_SIZE - 1;
    for (size_t i = 0; i < count; i++) {
        T value = a[i];
        for (size_t pass = 0; pass < hiPass; pass++) {
            freqs[pass][value & RADIX_MASK]++;
            value >>= RADIX_BITS;
        }
    }
}

// LSD radix sort implementation by Travis, see radix_sort_lsd.h
template <class T>
void radix_lsd(T* a, T* queue_area, size_t count, size_t hiPass)
{
    constexpr T RADIX_MASK = RADIX_SIZE - 1;

    freq_array_type freqs = {};
    count_frequency(a, count, freqs, hiPass);

    T *from = a, *to = queue_area;

    for (size_t pass = 0; pass < hiPass; pass++) {

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
            // I don't see impact
            //__builtin_prefetch(queue_ptrs[index] + 1);
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

    if (hi - lo < LSD_THRESHOLD) {
        radix_lsd(&from[0] + lo, &to[0] + lo, hi - lo, pass + 1);
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
void radix_sort_hybrid(std::vector<T>& data)
{
    std::vector<T> buf(data.size());
    details::radix_msd_rec(data, buf, 0, data.size(), details::RADIX_LEVELS - 1);
    if (details::DO_FINAL_SWAP)
        swap(data, buf);
}
