#include "EaseInOut.h"
#include <cmath>

EaseInOut::EaseInOut(double durationInSeconds, bool repeat, std::function<void()> onDone): TimingFunction(durationInSeconds, repeat, onDone) {}

double EaseInOut::ComputeTimingFunctionValue(double elapsedTimeInMilliseconds)
{
    return (1 + std::sin((M_PI/2) * (2*elapsedTimeInMilliseconds - _durationInMilliseconds) / _durationInMilliseconds) )/2;
}