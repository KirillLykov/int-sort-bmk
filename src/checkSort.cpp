#include <iostream>
#include <random>
#include <vector>

#include "boost_spread_sort.h"
#include "naiveradixsort.h"
#include "lsdradixsort_travis.h"
#include "msdradix.h"


// was lazy to install gtests on my personal machine
int main(int, char**)
{
    const auto n = 1000;
    std::vector<T> vals(n); //  = {6, 7, 11, 10, 6};

    std::default_random_engine generator;
    std::uniform_int_distribution<T> distribution(0, n);
    
    for (int i = 0; i < n; ++i) {
        vals[i] = distribution(generator);
    }

    std::shuffle(vals.begin(), vals.end(), std::mt19937{std::random_device{}()});

    for (auto& v : vals)
        std::cout << v << " ";
    std::cout << std::endl;

    //radixSort_count(vals);
    //integer_sort(vals.begin(), vals.end());

    //radix_sort6((uint64_t*)&vals.front(), vals.size());
    radix_sort_msd(vals);

    for (size_t i = 1; i < n; ++i) {
        if (vals[i - 1] > vals[i]) {
            std::cout << vals[i - 1] << " > " << vals[i] << std::endl;
            throw "something went wrong";
        }
    }

    return 0;
}
