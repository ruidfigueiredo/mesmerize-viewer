#pragma once
#include <chrono>

class TimingFunction
{
    private:
    static std::chrono::_V2::system_clock::time_point _startTime;
    protected:
    double _durationInMilliseconds;    
    virtual double ComputeTimingFunctionValue(double ellapsedTimeInMilliseconds) = 0;
    public:
    TimingFunction(double durationInSeconds);
    virtual ~TimingFunction();
    double GetValue();
};

