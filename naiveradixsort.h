#pragma once
#include <vector>
#include <array>

// T assumed silently to be integer
// TODO add template type checks
template<class T>
inline int getBits(T v, T i) { return (v >> i)&0b11; }

template<class T>
void radixSort_count(std::vector<T>& data, std::vector<T>& buf) {
    const int n = data.size();
    std::array<int, 8> psum = {0};
    constexpr auto sz = sizeof(T) * 8 - 2;
    for (T i = 0; i < sz; i += 2) {
        //count sort
        for (int v : data) {
            auto bits = getBits(v, i);
            ++psum[bits];
        }
        for (int i = 1; i < 8; ++i)
            psum[i] += psum[i-1];
        for (int j = n - 1; j>= 0; --j) {
            auto bits = getBits(data[j], i);
            --psum[bits];
            assert(psum[bits] < buf.size());
            buf[ psum[bits] ] = data[j];
        }
        swap(buf, data);
        psum = {0};
    }
}
