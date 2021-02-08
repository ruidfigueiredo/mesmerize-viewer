#include "ResolutionScaleCalculator.h"
#include <algorithm>

bool ResolutionScaleCalculator::IsScallingRequired(int width, int height, int maxDimension)
{
    return width > maxDimension || height > maxDimension;
}

std::pair<int, int> ResolutionScaleCalculator::ScaleToMaxDimension(int width, int height, int maxDimension)
{
    const float largestDimension = (float)std::max(width, height);
    const float scale = maxDimension / largestDimension;
    return std::pair<int, int>{(int)width * scale, (int)height * scale};
}

std::pair<int, int> ResolutionScaleCalculator::ScaleToDeviceResolution(int deviceWidth, int deviceHeight, int width, int height)
{
    const auto largestDimension = std::max(width, height);
    int targetMaxResolution;
    if (largestDimension == width)
        targetMaxResolution = deviceWidth;
    else
        targetMaxResolution = deviceHeight;

    const float scale = 1.0f * targetMaxResolution / largestDimension;
    return std::pair<int, int>{(int)width * scale, (int)height * scale};
}