#pragma once
#include "Picture.h"
#include "TimingFunctions/TimingFunction.h"
#include <memory>

enum class PictureRendererWithTransitionState {
    EMPTY,
    TRANSITION_COMPLETE,
    TRANSITION_IN_PROGRESS
};

class PictureRendererWithTransition {
    Picture _pictures[2];
    int _currentLoadedPictureIndex;
    std::shared_ptr<TimingFunction> _panTimingFunction;
    std::shared_ptr<TimingFunction> _zoomTimingFunction;
    std::shared_ptr<TimingFunction> _opacityTimingFunction;
    PictureRendererWithTransitionState _state;
    const float _percentageToMove;
    const float _percentageToZoom;
    int GetOccludedPictureIndex() const;
    void Swap();

public:
    PictureRendererWithTransition(int panAnimationDuration = 10000, int zoomAnimationDuration = 20000, int opacityAnimationDuration = 4000, float percentageToMove= 0.05f, float percentageToZoom = 0.05f);
    void Load(std::string path, std::function<void()> onLoaded = nullptr);
    void Render();
};

