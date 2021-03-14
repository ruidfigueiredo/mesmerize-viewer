#include "TimingFunction.h"
#include <iostream>

std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds> TimingFunction::_startTime = std::chrono::high_resolution_clock::now();

TimingFunction::TimingFunction(double durationInSeconds): _durationInSeconds(durationInSeconds) {}
TimingFunction::~TimingFunction() {}

double TimingFunction::GetValue() {
    auto now = std::chrono::high_resolution_clock::now();
    auto ellapsedDuration = now - _startTime;
    //std::cout << ellapsedDuration.count() << std::endl;
    std::chrono::duration<double> ellapsedDurationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(ellapsedDuration);
    std::cout << ellapsedDurationInMilliseconds.count() <<std::endl;
    
    auto value =  ComputeTimingFunctionValue(ellapsedDurationInMilliseconds.count());
    std::cout << value <<std::endl;
    
    return value;
}