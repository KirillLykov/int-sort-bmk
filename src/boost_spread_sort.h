// This file contains code copy-pasted from boost to make experiments
//
// Details for templated Spreadsort-based integer_sort.

//          Copyright Steven J. Ross 2001 - 2014.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// See http://www.boost.org/libs/sort for library home page.

#pragma once

#include <boost/static_assert.hpp>

namespace detail {
//Tuning constants
//This should be tuned to your processor cache;
//if you go too large you get cache misses on bins
//The smaller this number, the less worst-case memory usage.
//If too small, too many recursions slow down spreadsort
enum { max_splits = 13,
    //It's better to have a few cache misses and finish sorting
    //than to run another iteration
    max_finishing_splits = max_splits + 1,
    //Sets the minimum number of items per bin.
    int_log_mean_bin_size = 2,
    //Used to force a comparison-based sorting for small bins, if it's faster.
    //Minimum value 1
    int_log_min_split_count = 9,
    //This is the minimum split count to use spreadsort when it will finish in one
    //iteration.  Make this larger the faster boost::sort::pdqsort is relative to integer_sort.
    int_log_finishing_count = 31,
};

//This only works on unsigned data types
template <typename T>
inline unsigned
rough_log_2_size(const T& input)
{
    unsigned result = 0;
    //The && is necessary on some compilers to avoid infinite loops
    //it doesn't significantly impair performance
    while ((result < (8 * sizeof(T))) && (input >> result))
        ++result;
    return result;
}

//Gets the minimum size to call spreadsort on to control worst-case runtime.
//This is called for a set of bins, instead of bin-by-bin, to minimize
//runtime overhead.
//This could be replaced by a lookup table of sizeof(Div_type)*8 but this
//function is more general.
template <unsigned log_mean_bin_size,
    unsigned log_min_split_count, unsigned log_finishing_count>
inline size_t
get_min_count(unsigned log_range)
{
    const size_t typed_one = 1;
    const unsigned min_size = log_mean_bin_size + log_min_split_count;
    //Assuring that constants have valid settings
    BOOST_STATIC_ASSERT(log_min_split_count <= max_splits && log_min_split_count > 0);
    BOOST_STATIC_ASSERT(max_splits > 1 && max_splits < (8 * sizeof(unsigned)));
    BOOST_STATIC_ASSERT(max_finishing_splits >= max_splits && max_finishing_splits < (8 * sizeof(unsigned)));
    BOOST_STATIC_ASSERT(log_mean_bin_size >= 0);
    BOOST_STATIC_ASSERT(log_finishing_count >= 0);
    //if we can complete in one iteration, do so
    //This first check allows the compiler to optimize never-executed code out
    if (log_finishing_count < min_size) {
        if (log_range <= min_size && log_range <= max_splits) {
            //Return no smaller than a certain minimum limit
            if (log_range <= log_finishing_count)
                return typed_one << log_finishing_count;
            return typed_one << log_range;
        }
    }
    const unsigned base_iterations = max_splits - log_min_split_count;
    //sum of n to n + x = ((x + 1) * (n + (n + x)))/2 + log_mean_bin_size
    const unsigned base_range = ((base_iterations + 1) * (max_splits + log_min_split_count)) / 2
        + log_mean_bin_size;
    //Calculating the required number of iterations, and returning
    //1 << (iteration_count + min_size)
    if (log_range < base_range) {
        unsigned result = log_min_split_count;
        for (unsigned offset = min_size; offset < log_range;
             offset += ++result)
            ;
        //Preventing overflow; this situation shouldn't occur
        if ((result + log_mean_bin_size) >= (8 * sizeof(size_t)))
            return typed_one << ((8 * sizeof(size_t)) - 1);
        return typed_one << (result + log_mean_bin_size);
    }
    //A quick division can calculate the worst-case runtime for larger ranges
    unsigned remainder = log_range - base_range;
    //the max_splits - 1 is used to calculate the ceiling of the division
    unsigned bit_length = ((((max_splits - 1) + remainder) / max_splits)
        + base_iterations + min_size);
    //Preventing overflow; this situation shouldn't occur
    if (bit_length >= (8 * sizeof(size_t)))
        return typed_one << ((8 * sizeof(size_t)) - 1);
    //n(log_range)/max_splits + C, optimizing worst-case performance
    return typed_one << bit_length;
}

// Resizes the bin cache and bin sizes, and initializes each bin size to 0.
// This generates the memory overhead to use in radix sorting.
template <class RandomAccessIter>
inline RandomAccessIter*
size_bins(size_t* bin_sizes, std::vector<RandomAccessIter>& bin_cache, unsigned cache_offset, unsigned& cache_end, unsigned bin_count)
{
    // Clear the bin sizes
    for (size_t u = 0; u < bin_count; u++)
        bin_sizes[u] = 0;
    //Make sure there is space for the bins
    cache_end = cache_offset + bin_count;
    if (cache_end > bin_cache.size())
        bin_cache.resize(cache_end);
    return &(bin_cache[cache_offset]);
}

template <class RandomAccessIter>
inline bool
is_sorted_or_find_extremes(RandomAccessIter current, RandomAccessIter last,
    RandomAccessIter& max, RandomAccessIter& min)
{
    min = max = current;
    //This assumes we have more than 1 element based on prior checks.
    while (!(*(current + 1) < *current)) {
        //If everything is in sorted order, return
        if (++current == last - 1)
            return true;
    }

    //The maximum is the last sorted element
    max = current;
    //Start from the first unsorted element
    while (++current < last) {
        if (*max < *current)
            max = current;
        else if (*current < *min)
            min = current;
    }
    return false;
}

//Gets a non-negative right bit shift to operate as a logarithmic divisor
template <unsigned log_mean_bin_size>
inline int
get_log_divisor(size_t count, int log_range)
{
    int log_divisor;
    //If we can finish in one iteration without exceeding either
    //(2 to the max_finishing_splits) or n bins, do so
    if ((log_divisor = log_range - rough_log_2_size(count)) <= 0 && log_range <= max_finishing_splits)
        log_divisor = 0;
    else {
        //otherwise divide the data into an optimized number of pieces
        log_divisor += log_mean_bin_size;
        //Cannot exceed max_splits or cache misses slow down bin lookups
        if ((log_range - log_divisor) > max_splits)
            log_divisor = log_range - max_splits;
    }
    return log_divisor;
}

//Implementation for recursive integer sorting
template <class RandomAccessIter, class Div_type, class Size_type>
inline void
spreadsort_rec(RandomAccessIter first, RandomAccessIter last,
    std::vector<RandomAccessIter>& bin_cache, unsigned cache_offset, size_t* bin_sizes)
{
    //This step is roughly 10% of runtime, but it helps avoid worst-case
    //behavior and improve behavior with real data
    //If you know the maximum and minimum ahead of time, you can pass those
    //values in and skip this step for the first iteration
    RandomAccessIter max, min;
    if (is_sorted_or_find_extremes(first, last, max, min))
        return;
    RandomAccessIter* target_bin;
    unsigned log_divisor = get_log_divisor<int_log_mean_bin_size>(
        last - first, rough_log_2_size(Size_type((*max >> 0) - (*min >> 0))));
    Div_type div_min = *min >> log_divisor;
    Div_type div_max = *max >> log_divisor;
    unsigned bin_count = unsigned(div_max - div_min) + 1;

    unsigned cache_end;
    RandomAccessIter* bins = size_bins(bin_sizes, bin_cache, cache_offset, cache_end, bin_count);

    //Calculating the size of each bin; this takes roughly 10% of runtime
    for (RandomAccessIter current = first; current != last;)
        bin_sizes[size_t((*(current++) >> log_divisor) - div_min)]++;
    //Assign the bin positions
    bins[0] = first;
    for (unsigned u = 0; u < bin_count - 1; u++)
        bins[u + 1] = bins[u] + bin_sizes[u];

    RandomAccessIter nextbinstart = first;
    //Swap into place
    //This dominates runtime, mostly in the swap and bin lookups
    for (unsigned u = 0; u < bin_count - 1; ++u) {
        RandomAccessIter* local_bin = bins + u;
        nextbinstart += bin_sizes[u];

        //Iterating over each element in this bin
        for (RandomAccessIter current = *local_bin; current < nextbinstart;
             ++current) {
            // TODO(klykov): I think we can do it stable by avoid in-place here
            //Swapping elements in current into place until the correct
            //element has been swapped in
            for (target_bin = (bins + ((*current >> log_divisor) - div_min));
                 target_bin != local_bin;
                 target_bin = bins + ((*current >> log_divisor) - div_min)) {
                //3-way swap; this is about 1% faster than a 2-way swap
                //The main advantage is less copies are involved per item
                //put in the correct place
                typename std::iterator_traits<RandomAccessIter>::value_type tmp;
                RandomAccessIter b = (*target_bin)++;
                RandomAccessIter* b_bin = bins + ((*b >> log_divisor) - div_min);
                if (b_bin != local_bin) {
                    RandomAccessIter c = (*b_bin)++;
                    tmp = *c;
                    *c = *b;
                } else
                    tmp = *b;
                *b = *current;
                *current = tmp;
            }
        }
        *local_bin = nextbinstart;
    }
    bins[bin_count - 1] = last;

    //If we've bucketsorted, the array is sorted and we should skip recursion
    if (!log_divisor)
        return;
    //log_divisor is the remaining range; calculating the comparison threshold
    size_t max_count = get_min_count<int_log_mean_bin_size, int_log_min_split_count,
        int_log_finishing_count>(log_divisor);

    //Recursing
    RandomAccessIter lastPos = first;
    for (unsigned u = cache_offset; u < cache_end; lastPos = bin_cache[u],
                  ++u) {
        Size_type count = bin_cache[u] - lastPos;
        //don't sort unless there are at least two items to Compare
        if (count < 2)
            continue;
        //using boost::sort::pdqsort if its worst-case is better
        //if (count < max_count)
        //    boost::sort::pdqsort(lastPos, bin_cache[u]);
        //else
        spreadsort_rec<RandomAccessIter, Div_type, Size_type>(lastPos,
            bin_cache[u],
            bin_cache,
            cache_end,
            bin_sizes);
    }
}

//Holds the bin vector and makes the initial recursive call
template <class RandomAccessIter, class Div_type>
//Only use spreadsort if the integer can fit in a size_t
inline //typename boost::enable_if_c<sizeof(Div_type) <= sizeof(size_t),
    //void>::type
    void
    integer_sort(RandomAccessIter first, RandomAccessIter last, Div_type)
{
    size_t bin_sizes[1 << max_finishing_splits];
    std::vector<RandomAccessIter> bin_cache;
    spreadsort_rec<RandomAccessIter, Div_type, size_t>(first, last,
        bin_cache, 0, bin_sizes);
}
}

template <class RandomAccessIter>
inline void integer_sort(RandomAccessIter first, RandomAccessIter last)
{
    // Don't sort if it's too small to optimize.
    //if (last - first < detail::min_sort_size)
    //  boost::sort::pdqsort(first, last);
    //else
    detail::integer_sort(first, last, *first >> 0);
}
