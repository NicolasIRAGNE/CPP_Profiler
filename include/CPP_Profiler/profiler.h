/**
 * @file profiler.h
 * @author Nicolas IRAGNE (nicolas.iragne@alyce.fr)
 * @brief 
 * @date 2022-05-24
 * 
 * @copyright Copyright Alyce (c) 2022
 */

#include <boost/stacktrace.hpp>
#include <chrono>
#include <map>
#include <vector>
#include <stack>

namespace prof
{

template <typename T>
class Singleton
{
protected:
    Singleton() = default;
public:
    static T& getInstance()
    {
        static T instance;
        return instance;
    }
    Singleton(const Singleton&) = delete;
    Singleton& operator=(const Singleton&) = delete;
    Singleton(Singleton&&) = delete;
    Singleton& operator=(Singleton&&) = delete;
};


struct FunctionInfo
{
    std::string name;
    std::chrono::nanoseconds time = std::chrono::nanoseconds(0);
    int nb_call = 0;
    std::map<std::string, std::shared_ptr<FunctionInfo>> children;

    FunctionInfo(const std::string& name) : name(name) {}
};

class Profiler : public Singleton<Profiler>
{
    std::stack<std::chrono::high_resolution_clock::time_point> _time_stack;
    std::stack<std::shared_ptr<FunctionInfo>> _function_stack;
    std::vector<std::shared_ptr<FunctionInfo>> _root_functions;

public:
    Profiler();
    ~Profiler();

    /**
     * @brief Start a timer for function name
     * This will push a new pair in the stack
     * @param name The name of the function
     */
    void start_timer(const std::string &name);

    /**
     * @brief Stop a timer for function name
     * This will pop a pair from the stack and add the duration to the function info
     * @param name The name of the function
     */
    void stop_timer(const std::string &name);


    void print_report() const;
    void print_report(const FunctionInfo& info, int depth = 0) const;

    void to_json(const FunctionInfo& info, std::ostream& os) const;
    void to_json(std::ostream& os) const;
};

class ProfilerFrameObject
{
    std::string _name;

public:
    explicit ProfilerFrameObject(const std::string& name) : _name(name)
    {
        Profiler::getInstance().start_timer(_name);
    }

    ~ProfilerFrameObject()
    {
        Profiler::getInstance().stop_timer(_name);
    }
};

#define PROF_START \
    prof::ProfilerFrameObject __prof_frame_object__(__FUNCTION__);

} // namespace prof