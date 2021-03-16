#pragma once
#include <array>
#include <vector>

constexpr int npart = 4;
using T = int64_t;

// This one is faster
inline void partition_backward(const std::vector<T>& data, std::function<int(T)> partFunc, std::vector<T>& out)
{
    std::array<int, npart> psum = { 0 };
    //count sort
    for (T v : data) {
        ++psum[partFunc(v)];
    }
    for (int i = 1; i < npart; ++i)
        psum[i] += psum[i - 1];
    int n = (int)data.size();
    for (int j = n - 1; j >= 0; --j) {
        auto bits = partFunc(data[j]);
        --psum[bits];
        out[psum[bits]] = data[j];
    }
}

inline void partition_forward(const std::vector<T>& t_in, std::function<int(T)> partFunc, std::vector<T>& t_out)
{
    /// histogram
    std::array<int, npart> histogram = {};
    for (T v : t_in) {
        ++histogram[partFunc(v)];
    }

    /// partitioning
    std::array<int, npart> offset;
    int i = 0;
    for (int p = 0; p < npart; ++p) {
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

void radixSort_count(std::vector<T>& data, std::vector<T>& buf)
{
    const int n = data.size();
    constexpr auto sz = sizeof(T) * 8 - 2;
    for (T i = 0; i < sz; i += 2) {
        partition_backward(
            data, [i](T v) -> int { return (v >> i) & 0b11; }, buf);
        swap(buf, data);
    }
}
