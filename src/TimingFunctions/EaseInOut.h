#pragma once
#include <functional>
#include "TimingFunction.h"

class EaseInOut : public TimingFunction 
{
    double ComputeTimingFunctionValue(double elapsedTimeInMilliseconds);
    public: 
    EaseInOut(double durationInMilliseconds, bool repeat = true, std::function<void()> onDone = nullptr);
};