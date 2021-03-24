#pragma once
#include <vector>
#include <functional>


template<typename T>
class CallbackRegistry
{    struct CallbackInfo {
        T callback;
        void* owner;
    };
    std::vector<CallbackInfo> _callbacks;
public:
    void registerCallback(T callback, void* owner = nullptr);
    void unregisterCallbacksFor(void* owner);
    template<typename... Args>
    void invokeCallbacks(Args&&... args);
};

template<typename T>
void CallbackRegistry<T>::registerCallback(T callback, void *owner) {
    _callbacks.push_back(CallbackInfo {std::forward<T>(callback), owner});
}

template<typename T>
void CallbackRegistry<T>::unregisterCallbacksFor(void* owner) {
    std::vector<int> indexes;
    for(auto i = 0; i < _callbacks.size(); i++) {
        if (_callbacks[i].owner == owner){
            indexes.push_back(i);
        }
    }
    for(auto j = 0; j < indexes.size(); j++) {
        const auto indexToRemove = indexes[j];
        _callbacks.erase(_callbacks.begin() + indexToRemove - j); //as we remove indexes we have to adjust the position of the index to remove
    }
}
template<typename T>
template<typename... Args>
void CallbackRegistry<T>::invokeCallbacks(Args &&...args) {
    for(const auto & callbackInfo : _callbacks){
        callbackInfo.callback(std::forward<Args>(args)...);
    }
}