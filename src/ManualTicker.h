//
// Created by rdfi on 22/05/2021.
//
#include <chrono>
#include <functional>

#pragma once

class ManualTicker {
private:
    std::function<void()> _callback;
    std::chrono::_V2::system_clock::time_point _startTime;
    double _callbackIntervalInMs;

public:
    ManualTicker(std::function<void()> callback, double callbackIntervalInMs);
    void Start();
    void Tick();
};



