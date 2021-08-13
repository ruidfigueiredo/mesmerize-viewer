//
// Created by rdfi on 22/05/2021.
//
#include <chrono>
#include <functional>

#pragma once

class ManualTicker {
private:
    static constexpr double NO_TIMEOUT = -1;
    std::function<void()> _callback;
    std::chrono::_V2::system_clock::time_point _startTime;
    std::chrono::_V2::system_clock::time_point _pauseStartTime;
    double _callbackIntervalInMs;
    bool _isPaused;
    double _pauseTimeoutInMs;

public:
    ManualTicker(std::function<void()> callback, double callbackIntervalInMs);
    void Start();
    void Tick();
    void Pause(double pauseTimeoutInMs = NO_TIMEOUT);
    void Resume();
    void Reset();
};



