#pragma once
#include "Picture.h"
#include "TimingFunctions/TimingFunction.h"
#include <memory>

class PictureRendererWithTransition {
    Picture _pictures[2];
    int _currentPictureIndex;
    std::shared_ptr<TimingFunction> _timingFunction;
    const static int _animationDuration;
public:
    PictureRendererWithTransition();
    void Load(std::string path);
    void Render();
};

