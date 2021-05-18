#pragma once
#include "Picture.h"
#include "TimingFunctions/TimingFunction.h"
#include <memory>

class PictureRendererWithTransition {
    Picture _pictures[2];
    int _currentPictureIndex;
    std::shared_ptr<TimingFunction> _panTimingFunction;
    std::shared_ptr<TimingFunction> _zoomTimingFunction;
    std::shared_ptr<TimingFunction> _opacityTimingFunction;
    const static int _panAnimationDuration;
    const static int _zoomAnimationDuration;
    const static int _opacityAnimationDuration;
    const static float _percentageToMove;
    const static float _percentageToZoom;

public:
    PictureRendererWithTransition();
    void Load(std::string path);
    void Render();
};

