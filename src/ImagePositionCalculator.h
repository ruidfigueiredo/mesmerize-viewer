#pragma once
#include <array>
#include "ResolutionScaleCalculator.h"
#include <memory>

class ImagePositionCalculator
{
    std::shared_ptr<ResolutionScaleCalculator> _resolutionScaleCalculator;

public:
    ImagePositionCalculator(const std::shared_ptr<ResolutionScaleCalculator> resolutionScaleCalculator) : _resolutionScaleCalculator(resolutionScaleCalculator) {}
    std::array<float, 16> GetCenteredRectangleVertexCoordinates(int deviceWidth, int deviceHeight, int imageWidth, int imageHeight);
};
