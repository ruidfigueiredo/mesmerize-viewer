#pragma once
#include "Picture.h"
#include "TimingFunctions/TimingFunction.h"
#include <memory>

class PictureRendererWithTransition {
    Picture _pictures[2];
    int _currentPictureIndex;
    std::shared_ptr<TimingFunction> _panTimingFunction;
    std::shared_ptr<TimingFunction> _zoomTimingFunction;
    const static int _panAnimationDuration;
    const static int _zoomAnimationDuration;
public:
    PictureRendererWithTransition();
    void Load(std::string path);
    void Render();
};

