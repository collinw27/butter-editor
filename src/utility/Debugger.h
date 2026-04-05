#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <string>
#include <map>
#include <chrono>

using Clock = std::chrono::steady_clock;
using Second = std::chrono::duration<double, std::micro>;

class DebuggerSingleton;

DebuggerSingleton& Debugger();

class DebuggerSingleton
{
    struct ProfileData
    {
        Clock::time_point last;
        int since_message = 0;
        int accumulated = 0;
        int total = 0;
    };

    static DebuggerSingleton* singleton_object;

    std::map<std::string, ProfileData> profile_data {};

public:

    DebuggerSingleton();
    ~DebuggerSingleton();

    void profile_start(std::string category);
    void profile_end(std::string category, int print_interval);
    
    friend DebuggerSingleton& Debugger();
};

#endif