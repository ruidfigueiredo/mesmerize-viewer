#pragma once
#include <array>
#include <memory>
#include "ResolutionScaleCalculator.h"

struct PictureLoadResult
{
    bool WasLoadingSuccessful;
    std::array<int, 8> VertexCoordinates;
};

class PictureLoader
{
    unsigned int _mainTextureId;
    unsigned int _blurryBackgroundTextureId;
    std::shared_ptr<ResolutionScaleCalculator> _resolutionScaleCalculator;
    int _maxDimension;

public:
    PictureLoader(std::shared_ptr<ResolutionScaleCalculator> rsc, int maxDimension);
    PictureLoadResult Load(std::string pathToFile);
};