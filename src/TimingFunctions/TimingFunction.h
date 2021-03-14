#pragma once
#include <chrono>

class TimingFunction
{
    private:
    static std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> _startTime;
    protected:
    double _durationInSeconds;    
    virtual double ComputeTimingFunctionValue(double ellapsedTimeInSeconds) = 0;
    public:
    TimingFunction(double durationInSeconds);
    virtual ~TimingFunction();
    double GetValue();
};

