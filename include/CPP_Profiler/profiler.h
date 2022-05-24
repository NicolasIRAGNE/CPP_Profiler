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
    std::stack<std::chrono::high_resolution_clock::time_point> _time_stack; /// Stack to keep track of pushed times
    std::stack<std::shared_ptr<FunctionInfo>> _function_stack; /// Stack to keep track of function path
    std::vector<std::shared_ptr<FunctionInfo>> _root_functions; /// Root functions (functions that are not called by other functions)

public:
    ~Profiler();

    /**
     * @brief Start a timer for function name
     * This function should be called at the beginning of the function.
     * @param name The name of the function
     */
    void start_timer(const std::string &name);

    /**
     * @brief Stop a timer for function name
     * This should never be called directly, it is called automatically at the end of the function.
     * @param name The name of the function
     */
    void stop_timer(const std::string &name);

    void print_report() const;
    void print_report(const FunctionInfo& info, int depth = 0) const;

    void to_json(const FunctionInfo& info, std::ostream& os) const;
    void to_json(std::ostream& os) const;
};

/**
 * @brief Helper object used to start and stop timers on correct scope using constructor and destructor
 */
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