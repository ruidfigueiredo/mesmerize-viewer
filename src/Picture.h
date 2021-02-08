#pragma once
#include <array>
#include <memory>
#include "ResolutionScaleCalculator.h"

struct PictureLoadResult
{
    bool WasLoadingSuccessful;
    std::array<float, 16> VertexCoordinates;
};

class Picture
{
    unsigned int _mainTextureId;
    unsigned int _blurryBackgroundTextureId;
    std::shared_ptr<ResolutionScaleCalculator> _resolutionScaleCalculator;
    int _maxDimension;

public:
    Picture(std::shared_ptr<ResolutionScaleCalculator> rsc);
    PictureLoadResult Load(std::string pathToFile, int textureSlot);
};