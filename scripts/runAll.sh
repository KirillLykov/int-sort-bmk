#!/bin/bash

BMK="../build/src/sortBmks --benchmark_min_time=2 --benchmark_counters_tabular=true --benchmark_out_format=csv"

#$BMK --benchmark_filter=SortingBmk_allUnique/StdStableSort --benchmark_out=results/stdstablesort_allUnique.csv

$BMK --benchmark_filter=SortingBmk_allUnique/BoostSpreadSort --benchmark_out=results/boostspreadsort_allUnique.csv

$BMK --benchmark_filter=SortingBmk_allUnique/RadixSortMSD --benchmark_out=results/radixsortmsd_allUnique.csv

./plot.py -l1 "std::stable_sort" -f1 results/stdstablesort_allUnique.csv -l2 "boost::spreadsort" -f2 results/boostspreadsort_allUnique.csv -l3 "radix_sort_msd" -f3 results/radixsortmsd_allUnique.csv -o imgs/all_unique.png

## int64_t, uniform range [-n/2, n/2]
#../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_random_wholeRange/StdStableSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/stdstablesort_random_wholeRange.csv

#./plot.py -f1 results/stdstablesort_random_wholeRange.csv -f2 results/boostspreadsort_random_wholeRange.csv -f3 results/less_naivesort_random_wholeRange.csv -o imgs/all_random_wholeRange.png

