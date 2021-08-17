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
    const static int _panAnimationDuration;
    const static int _zoomAnimationDuration;
    const static int _opacityAnimationDuration;
    const static float _percentageToMove;
    const static float _percentageToZoom;
    int GetOccludedPictureIndex() const;
    void Swap();

public:
    PictureRendererWithTransition();
    void Load(std::string path, std::function<void()> onLoaded = nullptr);
    void Render();
};

