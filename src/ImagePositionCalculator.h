#pragma once
#include <array>
#include "ResolutionScaleCalculator.h"

class ImagePositionCalculator
{
public:
    std::array<float, 16> GetCenteredRectangleVertexCoordinates(int deviceWidth, int deviceHeight, int imageWidth, int imageHeight);
};
