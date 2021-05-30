#!/bin/bash

mkdir -p ./results

BMK="../build/src/allunique_bmk"
BMK_OPTIONS="--benchmark_min_time=2 --benchmark_counters_tabular=true --benchmark_out_format=csv"
CMD="$BMK $BMK_OPTIONS"

$CMD --benchmark_filter=SortingBmk_allUnique/StdStableSort --benchmark_out=results/stdstablesort_allUnique.csv

$CMD --benchmark_filter=SortingBmk_allUnique/BoostSpreadSort --benchmark_out=results/boostspreadsort_allUnique.csv

$CMD --benchmark_filter=SortingBmk_allUnique/LSDRadixSort --benchmark_out=results/radixsortlsd_allUnique.csv

$CMD --benchmark_filter=SortingBmk_allUnique/WolfSort --benchmark_out=results/wolfsort_allUnique.csv

./plot.py -l1 "std::stable_sort" -f1 results/stdstablesort_allUnique.csv -l2 "boost::spreadsort" -f2 results/boostspreadsort_allUnique.csv -l3 "radix_sort_lsd" -f3 results/radixsortlsd_allUnique.csv -l4 "wolfsort" -f4 results/wolfsort_allUnique.csv -o imgs/all_unique.png

$CMD --benchmark_filter=SortingBmk_uniform_1B/StdStableSort --benchmark_out=results/stdstablesort_uniform_1B.csv

$CMD --benchmark_filter=SortingBmk_uniform_1B/BoostSpreadSort --benchmark_out=results/boostspreadsort_uniform_1B.csv

$CMD --benchmark_filter=SortingBmk_uniform_1B/LSDRadixSort --benchmark_out=results/radixsortlsd_uniform_1B.csv

$CMD --benchmark_filter=SortingBmk_uniform_1B/WolfSort --benchmark_out=results/wolfsort_uniform_1B.csv

./plot.py -l1 "std::stable_sort" -f1 results/stdstablesort_uniform_1B.csv -l2 "boost::spreadsort" -f2 results/boostspreadsort_uniform_1B.csv -l3 "radix_sort_lsd" -f3 results/radixsortlsd_uniform_1B.csv -l4 "wolfsort" -f4 results/wolfsort_uniform_1B.csv -o imgs/uniform_1B.png

#BMK="../build/src/diffdistrib_bmk"
#BMK_OPTIONS="--benchmark_min_time=2 --benchmark_counters_tabular=true --benchmark_out_format=json"
#CMD="$BMK $BMK_OPTIONS"
#ALLBMKNAME=('SortingBmk_shuffled' 'SortingBmk_allequal' 'SortingBmk_ascending' 'SortingBmk_descending' 'SortingBmk_fewunique' 'SortingBmk_almostsorted')
#for BMKNAME in ${ALLBMKNAME[@]}; do
#    for BMKTYPE in 'StdStableSort' 'BoostSpreadSort' 'WolfSort' 'LSDRadixSort' 'HybridRadixSort'; do
#    $CMD --benchmark_filter=$BMKNAME/$BMKTYPE --benchmark_out=results/${BMKNAME}_${BMKTYPE}.json
#done
#done

#./plotBars.py -o imgs/differentdistrib.png
