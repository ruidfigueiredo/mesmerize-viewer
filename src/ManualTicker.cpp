//
// Created by rdfi on 22/05/2021.
//

#include "ManualTicker.h"

ManualTicker::ManualTicker(std::function<void()> callback, double callbackIntervalInMs): _callback(callback), _callbackIntervalInMs(callbackIntervalInMs), _isPaused(false), _pauseTimeoutInMs(NO_TIMEOUT) {}

void ManualTicker::Start(){

    _startTime = _startTime = std::chrono::high_resolution_clock::now();
}

void ManualTicker::Tick() {
    auto now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = now - _startTime;
    auto durationInMs = std::chrono::duration_cast<std::chrono::milliseconds>(duration);

    if (_isPaused && _pauseTimeoutInMs != NO_TIMEOUT) {
        std::chrono::duration<double> pauseDuration = now - _pauseStartTime;
        auto pauseDurationInMs = (std::chrono::duration_cast<std::chrono::milliseconds>(pauseDuration)).count();
        if (pauseDurationInMs >= _pauseTimeoutInMs) {
            Resume();
        }
    }

    if (durationInMs.count() >= _callbackIntervalInMs && !_isPaused){
        _callback();
        Start();
    }
}

void ManualTicker::Pause(double timeoutInMs) {
    _pauseTimeoutInMs = timeoutInMs;
    _pauseStartTime = std::chrono::high_resolution_clock::now();
    _isPaused = true;
}

void ManualTicker::Resume() {
    _isPaused = false;
    _pauseTimeoutInMs = NO_TIMEOUT;
}

void ManualTicker::Reset() {
    _startTime = _startTime = std::chrono::high_resolution_clock::now();
    Resume();
}


