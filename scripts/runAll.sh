#!/bin/bash

BMK="../build/src/allunique_bmk"
BMK_OPTIONS="--benchmark_min_time=2 --benchmark_counters_tabular=true --benchmark_out_format=csv"
CMD="$BMK $BMK_OPTIONS"

#$CMD --benchmark_filter=SortingBmk_allUnique/StdStableSort --benchmark_out=results/stdstablesort_allUnique.csv

#$CMD --benchmark_filter=SortingBmk_allUnique/BoostSpreadSort --benchmark_out=results/boostspreadsort_allUnique.csv

#$CMD --benchmark_filter=SortingBmk_allUnique/RadixSortMSD --benchmark_out=results/radixsortmsd_allUnique.csv

#./plot.py -l1 "std::stable_sort" -f1 results/stdstablesort_allUnique.csv -l2 "boost::spreadsort" -f2 results/boostspreadsort_allUnique.csv -l3 "radix_sort_msd" -f3 results/radixsortmsd_allUnique.csv -o imgs/all_unique.png

BMK="../build/src/diffdistrib_bmk"
BMK_OPTIONS="--benchmark_min_time=2 --benchmark_counters_tabular=true --benchmark_out_format=json"
CMD="$BMK $BMK_OPTIONS"
ALLBMKNAME=('SortingBmk_shuffled' 'SortingBmk_allequal' 'SortingBmk_ascending' 'SortingBmk_descending' 'SortingBmk_fewunique' 'SortingBmk_almostsorted')
for BMKNAME in ${ALLBMKNAME[@]}; do
    for BMKTYPE in 'StdStableSort' 'BoostSpreadSort' 'MSDRadixSort' 'LSDRadixSort'; do
    $CMD --benchmark_filter=$BMKNAME/$BMKTYPE --benchmark_out=results/${BMKNAME}_${BMKTYPE}.json
done
done

./plotBars.py -o imgs/differentdistrib.png
