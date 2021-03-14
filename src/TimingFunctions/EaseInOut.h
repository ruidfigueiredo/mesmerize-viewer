#pragma once
#include "TimingFunction.h"

class EaseInOut : public TimingFunction 
{        
    double ComputeTimingFunctionValue(double ellapsedTimeInSeconds);    
    public: 
    EaseInOut(double durationInSeconds);
};