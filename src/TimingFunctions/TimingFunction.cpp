#include "TimingFunction.h"

TimingFunction::TimingFunction(double durationInMilliseconds, bool repeat, std::function<void()> onDone): _durationInMilliseconds(durationInMilliseconds), _repeat(repeat) {
    _startTime = std::chrono::high_resolution_clock::now();
    _onDoneRegistry.registerCallback(onDone, this);
}
TimingFunction::~TimingFunction() {
    _onDoneRegistry.unregisterCallbacksFor(this);
}

double TimingFunction::GetValue() {
    auto now = std::chrono::high_resolution_clock::now();
    auto elapsedDuration = now - _startTime;
    auto elapsedDurationInMilliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(elapsedDuration).count();

    if (_repeat || elapsedDurationInMilliseconds <= _durationInMilliseconds) {
        return ComputeTimingFunctionValue(elapsedDurationInMilliseconds);
    }else {
        if (!_isDone) {
            _isDone = true; //this has to be before invoke callbacks has they might invoke reset
            _onDoneRegistry.invokeCallbacks();
        }
        return ComputeTimingFunctionValue(_durationInMilliseconds); //when repeat is false always returns end value
    }
}

void TimingFunction::Reset() {
    _isDone = false;
    _startTime = std::chrono::high_resolution_clock::now();
}