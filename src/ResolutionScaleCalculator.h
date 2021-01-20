#pragma once
#include <utility>

class ResolutionScaleCalculator
{
public:
    bool IsScallingRequired(int width, int height, int maxDimension);
    std::pair<int, int> ScaleToMaxDimension(int width, int height, int maxDimension);
};