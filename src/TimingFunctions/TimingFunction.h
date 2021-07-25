#pragma once
#include <chrono>
#include <functional>
#include "../CallbackRegistry.h"


class TimingFunction
{
    private:
    std::chrono::_V2::system_clock::time_point _startTime;
    CallbackRegistry<std::function<void()>> _onDoneRegistry;
    bool _repeat;
    bool _isDone;
    protected:
    double _durationInMilliseconds;    
    virtual double ComputeTimingFunctionValue(double elapsedTimeInMilliseconds) = 0;

    public:
    TimingFunction(double durationInSeconds, bool repeat, std::function<void()> onDone);
    virtual ~TimingFunction();
    double GetValue();
    void Reset();
};

