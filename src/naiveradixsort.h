#pragma once
#include <array>

#include <vector>

constexpr size_t radix_bits  = 8; // 8 gives x5 speed up over 2
constexpr size_t radix_size = 1ull << radix_bits;
constexpr size_t radix_levels = 63ull/radix_bits + 1;
constexpr size_t radix_mask = radix_size - 1;

using T = uint64_t;

// This one is faster
inline void partition_backward(const std::vector<T>& data, std::function<int(T)> partFunc, std::vector<T>& out)
{
    std::array<int, radix_size> psum = { 0 };
    //count sort
    for (T v : data) {
        ++psum[partFunc(v)];
    }
    for (int i = 1; i < radix_size; ++i)
        psum[i] += psum[i - 1];
    int n = (int)data.size();
    for (int j = n - 1; j >= 0; --j) {
        auto bits = partFunc(data[j]);
        --psum[bits];
        out[psum[bits]] = data[j];
    }
}

void radixSort_orig(std::vector<T>& data)
{
    std::vector<T> buf(data.size());
    const int n = data.size();
    constexpr auto sz = sizeof(T) * 8 - 2;

    for (T shift = 0, it = 0; shift < sz; shift += radix_bits, ++it) {
        partition_backward(data, [shift](T v) -> int { return (v >> shift) & radix_mask; }, buf);
        swap(buf, data);
    }
}

static bool hasOneBucket(size_t freqs[radix_size], size_t count) {
    for (size_t i = 0; i < radix_size; i++) {
        auto freq = freqs[i];
        if (freq != 0) {
            return freq == count;
        }
    }
    return true;
}

void radixSort_count(std::vector<T>& data)
{
    std::vector<T> buf(data.size());
    const int n = data.size();
    constexpr auto sz = sizeof(T) * 8 - 2;
    constexpr auto partFunc = [](T v, size_t i) -> int { return (v >> i) & radix_mask; }; // it looks to be inlined

    size_t hist[radix_levels][radix_size] = {};
    for (T shift = 0, it = 0; shift < sz; shift += radix_bits, ++it) {
        // it == shift / radix_bits but it is easier to have it here
        for (T v : data) {
            ++hist[it][partFunc(v, shift)];
        }
    }

    for (T shift = 0, it = 0; shift < sz; shift += radix_bits, ++it) {

        if (hasOneBucket(hist[it], n))
            continue;

        // Using pointers directly gives 24% speed up in comparison with array of offsets
        // backward way looks a bit faster
        //
        T* bucketPtrs[radix_size];
        bucketPtrs[0] = &buf.front() + hist[it][0];
        for (int i = 1; i < radix_size; ++i) {
            bucketPtrs[i] = bucketPtrs[i-1]  + hist[it][i];
        }
        int n = (int)data.size();
        for (int j = n - 1; j >= 0; --j) {
            auto bits = partFunc(data[j], shift);
            auto& where = bucketPtrs[bits];
            --where;
            *where = data[j];
        }
        /*for (auto& v : buf)
            std::cout << v << " ";
        std::cout << std::endl;*/
        swap(buf, data);
    }
    
    if (radix_levels & 1)
        std::copy(buf.begin(), buf.end(), data.begin());
}

// Slower
inline void partition_forward(const std::vector<T>& t_in, std::function<int(T)> partFunc, std::vector<T>& t_out)
{
    /// histogram
    std::array<int, radix_size> histogram = {};
    for (T v : t_in) {
        ++histogram[partFunc(v)];
    }

    /// partitioning
    std::array<int, radix_size> offset;
    int i = 0;
    for (int p = 0; p < radix_size; ++p) {
        offset[p] = i;
        i += histogram[p];
    }
    for (int i_in = 0; i_in < t_in.size(); ++i_in) {
        auto key = t_in[i_in]; // key and data are the same simplicity
        int i_out = offset[partFunc(key)];
        ++offset[partFunc(key)];
        t_out[i_out] = key;
    }
}
