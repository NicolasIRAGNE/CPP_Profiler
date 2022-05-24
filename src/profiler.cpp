/**
 * @file profiler.cpp
 * @author Nicolas IRAGNE (nicolas.iragne@alyce.fr)
 * @brief
 * @date 2022-05-24
 *
 * @copyright Copyright Alyce (c) 2022
 */

#include "profiler.h"
#include <functional>
#include <iostream>


namespace prof
{

Profiler::~Profiler()
{
    print_report();
}

void Profiler::start_timer(const std::string& name)
{
    _time_stack.emplace(std::chrono::high_resolution_clock::now());
    if (_function_stack.empty())
    {
        auto ptr = std::make_shared<FunctionInfo>(name);
        _function_stack.emplace(ptr);
        _root_functions.emplace_back(ptr);
    }
    else
    {
        auto& caller = _function_stack.top();
        auto& info = caller->children[name];
        if (info == nullptr)
        {
            info = std::make_shared<FunctionInfo>(name);
        }
        _function_stack.emplace(info);
    }
}

void Profiler::stop_timer(const std::string& name)
{
    auto end = std::chrono::high_resolution_clock::now();
    const auto& start = _time_stack.top();
    _time_stack.pop();
    const auto& info = _function_stack.top();
    info->time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    info->nb_call++;
    _function_stack.pop();
}

void Profiler::print_report() const
{
    std::cout << "Profiler report:" << std::endl;
    for (auto& root_function : _root_functions)
    {
        print_report(*root_function, 0);
    }
}

void Profiler::print_report(const FunctionInfo& info, int depth) const
{
    auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(info.time);
    std::cout << std::string(depth * 2, ' ') << info.name << ": " << duration_ms.count() << "ms (" << info.nb_call << " calls)" << std::endl;
    for (auto& child : info.children)
    {
        print_report(*child.second, depth + 1);
    }
}

void Profiler::to_json(const FunctionInfo& info, std::ostream& os) const
{
    os << "{";
    os << "\"name\": \"" << info.name << "\",";
    os << "\"time\": " << info.time.count() << ",";
    os << "\"nb_call\": " << info.nb_call << ",";
    os << "\"children\": [";
    bool first = true;
    for (auto& child : info.children)
    {
        if (!first)
        {
            os << ",";
        }
        to_json(*child.second, os);
        first = false;
    }
    os << "]";
    os << "}";
}

void Profiler::to_json(std::ostream& os) const
{
    os << "[";
    bool first = true;
    for (auto& root_function : _root_functions)
    {
        if (!first)
        {
            os << ",";
        }
        to_json(*root_function, os);
        first = false;
    }
    os << "]";
}

} // namespace prof