# sorting bmks
add_executable (sortBmks sortBmks.cpp)
target_link_libraries(sortBmks benchmark::benchmark)

add_executable (checkSort checkSort.cpp)
