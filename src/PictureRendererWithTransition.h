#pragma once
#include "Picture.h"

class PictureRendererWithTransition {
    Picture _pictures[2];
    int _currentPictureIndex;
public:
    PictureRendererWithTransition();
    void Load(std::string path);
    void Render();
};

