#!/bin/bash

## int64_t, all unique
../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_allUnique/StdStableSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/stdstablesort_allUnique.csv

../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_allUnique/BoostSpreadSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/boostspreadsort_allUnique.csv

../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_allUnique/NaiveRadixSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/naivesort_allUnique.csv

./plot.py -f1 results/stdstablesort_allUnique.csv -f2 results/boostspreadsort_allUnique.csv -f3 results/naivesort_allUnique.csv -o imgs/all_unique.png

## int64_t, uniform range [-n/2, n/2]
../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_random_wholeRange/StdStableSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/stdstablesort_random_wholeRange.csv

../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_random_wholeRange/BoostSpreadSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/boostspreadsort_random_wholeRange.csv

../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_random_wholeRange/NaiveRadixSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/naivesort_random_wholeRange.csv

./plot.py -f1 results/stdstablesort_random_wholeRange.csv -f2 results/boostspreadsort_random_wholeRange.csv -f3 results/naivesort_random_wholeRange.csv -o imgs/all_random_wholeRange.png

## int64_t, uniform range [-n/8, n/8]
../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_random_1d4Range/StdStableSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/stdstablesort_random_1d4Range.csv

../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_random_1d4Range/BoostSpreadSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/boostspreadsort_random_1d4Range.csv

../build/src/sortBmks  --benchmark_min_time=2 --benchmark_filter=SortingBmk_random_1d4Range/NaiveRadixSort --benchmark_counters_tabular=true --benchmark_out_format=csv --benchmark_out=results/naivesort_random_1d4Range.csv

./plot.py -f1 results/stdstablesort_random_1d4Range.csv -f2 results/boostspreadsort_random_1d4Range.csv -f3 results/naivesort_random_1d4Range.csv -o imgs/all_random_1d4Range.png

