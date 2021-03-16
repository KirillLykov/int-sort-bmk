cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

project(sortBmks)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -O3 -g3")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" ${CMAKE_MODULE_PATH})

if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_definitions("-DBOOST_DISABLE_ASSERTS -DEIGEN_NO_DEBUG")
endif()

find_package(benchmark REQUIRED)
# we use header-only sorting algorithms from boost
find_package(Boost ${BoostAlt_FIND_VERSION_OPTIONS} COMPONENTS system REQUIRED)

include_directories(${Boost_INCLUDE_DIR})

add_subdirectory(src)

#add_executable (sortBmks sortBmks.cpp)
#target_link_libraries(sortBmks benchmark::benchmark)
