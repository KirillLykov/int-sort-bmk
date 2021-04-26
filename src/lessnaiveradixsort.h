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

#include <x86intrin.h>

/* HEDLEY_INLINE */
#define HEDLEY_INLINE inline
/* HEDLEY_NEVER_INLINE */
#  define HEDLEY_NEVER_INLINE __attribute__((__noinline__))

const size_t    RADIX_BITS   = 8;
const size_t    RADIX_SIZE   = (size_t)1 << RADIX_BITS;
const size_t    RADIX_LEVELS = (63 / RADIX_BITS) + 1;
const uint64_t  RADIX_MASK   = RADIX_SIZE - 1;

using freq_array_type = size_t [RADIX_LEVELS][RADIX_SIZE];

// never inline just to make it show up easily in profiles (inlining this lengthly function doesn't
// really help anyways)
HEDLEY_NEVER_INLINE
static void count_frequency(uint64_t *a, size_t count, freq_array_type freqs) {
  for (size_t i = 0; i < count; i++) {
      uint64_t value = a[i];
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
static bool is_trivial(size_t freqs[RADIX_SIZE], size_t count) {
    for (size_t i = 0; i < RADIX_SIZE; i++) {
        auto freq = freqs[i];
        if (freq != 0) {
            return freq == count;
        }
    }
    assert(count == 0); // we only get here if count was zero
    return true;
}

inline void radix_sort7(uint64_t *a, size_t count)
{
    std::unique_ptr<uint64_t[]> queue_area(new uint64_t[count]);
    // huge_unique_ptr<uint64_t[]> queue_area = make_huge_array<uint64_t>(count, false);

    freq_array_type freqs = {};
    count_frequency(a, count, freqs);

    uint64_t *from = a, *to = queue_area.get();

    for (size_t pass = 0; pass < RADIX_LEVELS; pass++) {

        if (is_trivial(freqs[pass], count)) {
            // this pass would do nothing, just skip it
            continue;
        }

        uint64_t shift = pass * RADIX_BITS;

        // array of pointers to the current position in each queue, which we set up based on the
        // known final sizes of each queue (i.e., "tighly packed")
        uint64_t * queue_ptrs[RADIX_SIZE], * next = to;
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
            __builtin_prefetch(queue_ptrs[index] + 1);
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
void radix_sort6(std::vector<uint64_t>& aa)
{
    constexpr auto partFunc = [](T v, size_t i) -> int { return (v >> i) & radix_mask; }; // it looks to be inlined

    const auto count = aa.size();
    auto a = &aa[0];
    std::vector<uint64_t> queue_area(count);

    //  moving hist inside main loop makes execution x2 slower
    freq_array_type freqs = {};
    for (size_t i = 0; i < count; i++) {
      uint64_t value = a[i];
      for (size_t pass = 0; pass < RADIX_LEVELS; pass++) {
          freqs[pass][partFunc(value, pass * RADIX_BITS)]++;
      }
    }

    uint64_t *from = a, *to = &queue_area[0];

    for (size_t pass = 0; pass < RADIX_LEVELS; pass++) {

        if (is_trivial(freqs[pass], count)) {
            // this pass would do nothing, just skip it
            continue;
        }

        uint64_t shift = pass * RADIX_BITS;

        // array of pointers to the current position in each queue, which we set up based on the
        // known final sizes of each queue (i.e., "tighly packed")
        uint64_t * queue_ptrs[RADIX_SIZE], * next = to;
        //queue_ptrs[0] = to;
        for (size_t i = 0; i < RADIX_SIZE; i++) {
            //queue_ptrs[i] = queue_ptrs[i-1] + freqs[pass][i]; i - 1?
            queue_ptrs[i] = next;
            next += freqs[pass][i];
        }

        // copy each element into the appropriate queue based on the current RADIX_BITS sized
        // "digit" within it
        for (size_t i = 0; i < count; i++) {
            size_t value = from[i];
            size_t index = partFunc(value, shift);
            //*queue_ptrs[index]++ = value;
            *queue_ptrs[index] = value;
            ++queue_ptrs[index];
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

void radix_sort_msd(std::vector<uint64_t>& aa)
{
    constexpr auto partFunc = [](T v, size_t i) -> int { return (v >> i) & radix_mask; }; // it looks to be inlined

    const auto count = aa.size();
    auto a = &aa[0];
    std::vector<uint64_t> queue_area(count);

    uint64_t *from = a, *to = &queue_area[0];

    std::stack<std::tuple<int,size_t, uint64_t*, uint64_t*>> st; // pass, count, toLo, fromLo
    st.emplace(RADIX_LEVELS - 1, count, to, from);
    //for (size_t pass = 0; pass < RADIX_LEVELS; pass++) {
    while (!st.empty()) {
        auto[pass, count, toLo, fromLo] = st.top();
        st.pop();

        uint64_t shift = pass * RADIX_BITS;

        size_t freqs[RADIX_SIZE] = {};
        for (size_t i = 0; i < count; i++) {
            uint64_t value = fromLo[i];
            ++freqs[partFunc(value, shift)];
        }

        if (is_trivial(freqs, count)) {
            // this pass would do nothing, just skip it
            if (pass + 1 < RADIX_LEVELS)
                st.emplace(pass + 1, lo, hi, toLo, fromLo);
            continue;
        }


        // array of pointers to the current position in each queue, which we set up based on the
        // known final sizes of each queue (i.e., "tighly packed")
        uint64_t * queue_ptrs[RADIX_SIZE], * next = toLo;
        for (size_t i = 0; i < RADIX_SIZE; i++) {
            queue_ptrs[i] = next;
            next += freqs[i];
        }

        // copy each element into the appropriate queue based on the current RADIX_BITS sized
        // "digit" within it
        for (size_t i = 0; i < count; i++) {
            size_t value = fromLo[i];
            size_t index = partFunc(value, shift);
            //*queue_ptrs[index]++ = value;
            *queue_ptrs[index] = value;
            ++queue_ptrs[index];
        }
        // swap when we change the level
        if (prevPass < pass) {
            prevPass = pass;
            std::swap(fromLo, toLo);
        }
        if (pass + 1 < RADIX_LEVELS)
            for (size_t i = 0; i < RADIX_SIZE; i++) {
            st.emplace(pass + 1, lo, hi, toLo, fromLo);
    }

    // because of the last swap, the "from" area has the sorted payload: if it's
    // not the original array "a", do a final copy
    if (from != a) {
        std::copy(from, from + count, a);
    }
}
