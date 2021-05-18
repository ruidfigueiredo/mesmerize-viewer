#include "EaseInOut.h"
#include <cmath>

EaseInOut::EaseInOut(double durationInSeconds, bool repeat, std::function<void()> onDone): TimingFunction(durationInSeconds, repeat, onDone) {}

double EaseInOut::ComputeTimingFunctionValue(double elapsedTimeInMilliseconds)
{
    return std::abs(std::sin((M_PI/2) * elapsedTimeInMilliseconds / _durationInMilliseconds));
}