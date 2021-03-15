#include "EaseInOut.h"
#include <cmath>

EaseInOut::EaseInOut(double durationInSeconds): TimingFunction(durationInSeconds) {}

double EaseInOut::ComputeTimingFunctionValue(double ellapsedTimeInMilliseconds)
{
    return std::abs(std::sin((M_PI/2)*ellapsedTimeInMilliseconds/_durationInMilliseconds));
}