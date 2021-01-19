#pragma once
#include <utility>

class ResolutionScaleCalculator
{
public:
    std::pair<int, int> ScaleToMaxDimension(int width, int height, int maxDimension);
};