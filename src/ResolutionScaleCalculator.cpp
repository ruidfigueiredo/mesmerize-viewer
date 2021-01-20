#include "ResolutionScaleCalculator.h"
#include <algorithm>

bool IsScallingRequired(int width, int height, int maxDimension)
{
}

std::pair<int, int> ResolutionScaleCalculator::ScaleToMaxDimension(int width, int height, int maxDimension)
{
    const float largestDimension = (float)std::max(width, height);
    const float scale = maxDimension / largestDimension;
    return std::pair<int, int>{(int)width * scale, (int)height * scale};
}