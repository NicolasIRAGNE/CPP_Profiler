cmake_minimum_required(VERSION 3.18)

project(CPP_Profiler LANGUAGES CXX)

add_library(cpp_profiler)

find_package(Boost COMPONENTS system REQUIRED)

target_sources(cpp_profiler
    PRIVATE
        src/profiler.cpp
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/CPP_Profiler/profiler.h>
)

target_include_directories(cpp_profiler
    PRIVATE
        src
    PUBLIC
        include/CPP_Profiler
)

target_link_libraries(cpp_profiler
    PUBLIC
        Boost::system
)

add_executable(profiler_test src/main.cpp)

target_link_libraries(profiler_test
    PRIVATE
        cpp_profiler
)
