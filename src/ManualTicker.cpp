//
// Created by rdfi on 22/05/2021.
//

#include "ManualTicker.h"

ManualTicker::ManualTicker(std::function<void()> callback, double callbackIntervalInMs): _callback(callback), _callbackIntervalInMs(callbackIntervalInMs) {}

void ManualTicker::Start(){

    _startTime = _startTime = std::chrono::high_resolution_clock::now();
}

void ManualTicker::Tick() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = now - _startTime;
    auto durationInMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    if (durationInMs.count() >= _callbackIntervalInMs){
        _callback();
        Start();
    }
}


