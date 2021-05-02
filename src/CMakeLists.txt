# sorting bmks
add_executable (allunique_bmk allunique_bmk.cpp)
target_link_libraries(allunique_bmk benchmark::benchmark)

add_executable (diffdistrib_bmk diffdistrib_bmk.cpp)
target_link_libraries(diffdistrib_bmk benchmark::benchmark)

add_executable (checkSort checkSort.cpp)
