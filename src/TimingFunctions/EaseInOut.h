#pragma once
#include "TimingFunction.h"

class EaseInOut : public TimingFunction 
{        
    double ComputeTimingFunctionValue(double elapsedTimeInMilliseconds);
    public: 
    EaseInOut(double durationInMilliseconds);
};