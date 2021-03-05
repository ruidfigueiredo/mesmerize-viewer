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

std::pair<int, int> ResolutionScaleCalculator::ScaleToCover(int deviceWidth, int deviceHeight, int width, int height)
{
    float scale = 1.0f;
    scale = 1.0f * deviceWidth / width;
    scale = std::max(scale, 1.0f * deviceHeight / height);

    return std::pair<int, int>{(int)width * scale, (int)height * scale};
}

std::pair<int, int> ResolutionScaleCalculator::ScaleToFit(int deviceWidth, int deviceHeight, int width, int height)
{
    float scale = 1.0f;
    scale = 1.0f * deviceWidth / width;
    scale = std::min(scale, 1.0f * deviceHeight / height);

    return std::pair<int, int>{(int)width * scale, (int)height * scale};
}