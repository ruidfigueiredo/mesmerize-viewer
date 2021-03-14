#include "TimingFunction.h"
#include <iostream>

std::chrono::_V2::system_clock::time_point TimingFunction::_startTime = std::chrono::high_resolution_clock::now();

TimingFunction::TimingFunction(double durationInMilliseconds): _durationInMilliseconds(durationInMilliseconds) {}
TimingFunction::~TimingFunction() {}

double TimingFunction::GetValue() {
    auto now = std::chrono::high_resolution_clock::now();
    auto ellapsedDuration = now - _startTime;
    std::cout << "ellapsedTime: " << ellapsedDuration.count() << std::endl;
    auto ellapsedDurationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(ellapsedDuration);
    std::cout << "ellapsedTimeInMilliseconds: " << ellapsedDurationInMilliseconds.count() <<std::endl;
    
    auto value =  ComputeTimingFunctionValue(ellapsedDurationInMilliseconds.count());
    std::cout << "Value: " << value <<std::endl;
    
    return value;
}