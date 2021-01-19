#include <gtest/gtest.h>
#include "../ResolutionScaleCalculator.h"

TEST(ResolutionScaleCalculator, MaxDimensionEqualToWidthAndHeightReturnsWithAndHeihgt)
{
    ResolutionScaleCalculator rsc;

    auto result = rsc.ScaleToMaxDimension(100, 100, 100);

    ASSERT_EQ(result.first, 100);
    ASSERT_EQ(result.second, 100);
}

TEST(ResolutionScaleCalculator, SameWidthHeightLessThanMaxDimensionScalesUpToMaxDimension)
{
    ResolutionScaleCalculator rsc;

    auto result = rsc.ScaleToMaxDimension(50, 50, 100);

    ASSERT_EQ(result.first, 100);
    ASSERT_EQ(result.second, 100);
}

TEST(ResolutionScaleCalculator, HalftHeightThanWidthLessThanMaxDimensionScalesUpToMaxDimension)
{
    ResolutionScaleCalculator rsc;

    auto result = rsc.ScaleToMaxDimension(50, 25, 100);

    ASSERT_EQ(result.first, 100);
    ASSERT_EQ(result.second, 50);
}

TEST(ResolutionScaleCalculator, DoubleHeightThanWidthMoreThanMaxDimensionScalesDownToMaxDimension)
{
    ResolutionScaleCalculator rsc;

    auto result = rsc.ScaleToMaxDimension(800, 400, 200);

    ASSERT_EQ(result.first, 200);
    ASSERT_EQ(result.second, 100);
}
