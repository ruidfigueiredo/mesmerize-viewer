#pragma once
#include "TimingFunction.h"

class EaseInOut : public TimingFunction 
{        
    double ComputeTimingFunctionValue(double ellapsedTimeInMilliseconds);    
    public: 
    EaseInOut(double durationInMilliseconds);
};