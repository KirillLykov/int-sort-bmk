#include <iostream>
#include <random>
#include <vector>

#include "boost_spread_sort.h"
#include "naiveradixsort.h"
#include "lessnaiveradixsort.h"

using T = int64_t;

// was lazy to install gtests on my personal machine
int main(int, char**)
{
    const auto n = 1000;
    std::vector<T> vals(n);

    std::default_random_engine generator;
    std::uniform_int_distribution<T> distribution(0, n);

    for (int i = 0; i < n; ++i) {
        vals[i] = distribution(generator);
    }

    std::vector<T> buffer(n);

    //radixSort_count(vals, buffer);
    //integer_sort(vals.begin(), vals.end());

    radix_sort7((uint64_t*)&vals.front(), vals.size());

    for (size_t i = 1; i < n; ++i) {
        if (vals[i - 1] > vals[i]) {
            std::cout << vals[i - 1] << " > " << vals[i] << std::endl;
            throw "something went wrong";
        }
    }

    return 0;
}
