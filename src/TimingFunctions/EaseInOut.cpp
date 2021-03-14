#include "EaseInOut.h"
#include <cmath>

EaseInOut::EaseInOut(double durationInSeconds): TimingFunction(durationInSeconds) {}

double EaseInOut::ComputeTimingFunctionValue(double ellapsedTimeInSeconds)
{
    return std::sin((M_PI/2)*ellapsedTimeInSeconds/_durationInSeconds);
}