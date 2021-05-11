#include "EaseInOut.h"
#include <cmath>

EaseInOut::EaseInOut(double durationInSeconds): TimingFunction(durationInSeconds) {}

double EaseInOut::ComputeTimingFunctionValue(double elapsedTimeInMilliseconds)
{
    return std::abs(std::sin((M_PI/2) * elapsedTimeInMilliseconds / _durationInMilliseconds));
}