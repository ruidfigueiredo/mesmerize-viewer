#include "TimingFunction.h"

std::chrono::_V2::system_clock::time_point TimingFunction::_startTime = std::chrono::high_resolution_clock::now();

TimingFunction::TimingFunction(double durationInMilliseconds): _durationInMilliseconds(durationInMilliseconds) {}
TimingFunction::~TimingFunction() {}

double TimingFunction::GetValue() {
    auto now = std::chrono::high_resolution_clock::now();
    auto ellapsedDuration = now - _startTime;
    auto ellapsedDurationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(ellapsedDuration);
    
    return ComputeTimingFunctionValue(ellapsedDurationInMilliseconds.count());
}