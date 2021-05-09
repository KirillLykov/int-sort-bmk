#include <iostream>
#include <random>
#include <vector>

#include "radix_sort_lsd.h"
#include "radix_sort_msd.h"
#include "radix_sort_hybrid.h"

// was lazy to install gtests on my personal machine
int main(int, char**)
{
    using T = uint64_t;
    const auto n = 1000;
    std::vector<T> vals(n); //  = {6, 7, 11, 10, 6};

    std::default_random_engine generator;
    std::uniform_int_distribution<T> distribution(0, n);

    for (int i = 0; i < n; ++i) {
        vals[i] = distribution(generator);
    }

    std::shuffle(vals.begin(), vals.end(), std::mt19937 { std::random_device {}() });

    radix_sort_hybrid(vals);

    for (size_t i = 1; i < n; ++i) {
        if (vals[i - 1] > vals[i]) {
            std::cout << vals[i - 1] << " > " << vals[i] << std::endl;
            throw "something went wrong";
        }
    }

    return 0;
}
