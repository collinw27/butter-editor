#include "utility/Debugger.h"

#include "utility/Logger.h"
#include "utility/Exceptions.h"

DebuggerSingleton* DebuggerSingleton::singleton_object = nullptr;

DebuggerSingleton& Debugger()
{
    return *DebuggerSingleton::singleton_object;
}

DebuggerSingleton::DebuggerSingleton()
{
    if (DebuggerSingleton::singleton_object != nullptr)
        throw ButterException("Reinitialization of singleton");
    DebuggerSingleton::singleton_object = this;
}

DebuggerSingleton::~DebuggerSingleton()
{
    DebuggerSingleton::singleton_object = nullptr;
}

void DebuggerSingleton::profile_start(std::string category)
{
    auto it = profile_data.find(category);
    ProfileData data = (it == profile_data.end()) ? ProfileData{} : it->second;
    data.last = Clock::now();
    profile_data[category] = data;
}

void DebuggerSingleton::profile_end(std::string category, int print_interval)
{
    auto it = profile_data.find(category);
    if (it == profile_data.end())
        throw ButterException("Invalid profile '" + category + "'");
    ProfileData& data = it->second;
    int elapsed = (int)std::chrono::duration_cast<Second>(Clock::now() - data.last).count();
    data.accumulated += elapsed;
    data.since_message += elapsed;
    ++data.total;

    if (data.since_message >= print_interval)
    {
        float avg = (float)data.accumulated / data.total;
        Logger().log(std::stringstream{} << "[" << category << "] Average = " << avg << " us");
        data.accumulated = 0;
        data.since_message = 0;
        data.total = 0;
    }
}