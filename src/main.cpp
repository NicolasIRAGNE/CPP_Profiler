/**
 * @file main.cpp
 * @author Nicolas IRAGNE (nicolas.iragne@alyce.fr)
 * @brief
 * @date 2022-05-24
 *
 * @copyright Copyright Alyce (c) 2022
 */

#include "profiler.h"
#include <iostream>
#include <fstream>
#include <thread>

void f()
{
    PROF_START
}

void g()
{
    PROF_START
    f();
}

void test_function()
{
    PROF_START
    std::this_thread::sleep_for(std::chrono::milliseconds(66));
}

void test_prof()
{
    PROF_START
    test_function();
    test_function();
    f();
    f();
    g();
}

auto fib(int n)
{
    PROF_START
    if (n <= 1)
        return 1;
    return fib(n - 1) + fib(n - 2);
}

int main(int argc, char* argv[])
{
    PROF_START
    std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();
    test_prof();
    fib(20);
    std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock::now();
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "Chrono: " << duration_ms.count() << " ms" << std::endl;
}