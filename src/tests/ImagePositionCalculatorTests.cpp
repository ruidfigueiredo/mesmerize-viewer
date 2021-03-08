#include <gtest/gtest.h>
#include "../ResolutionScaleCalculator.h"
#include "../ImagePositionCalculator.h"
#include <memory>

TEST(ImagePositionCalculator, ImageMatchesMaxResolutionReturnsRectangleWithExactlyTheImageDimentions)
{
    ImagePositionCalculator imagePositionCalculator;

    auto res = imagePositionCalculator.GetCenteredRectangleVertexCoordinates(1920, 1080, 1920, 1080);

    ASSERT_EQ(res[0], 0);
    ASSERT_EQ(res[1], 0);
    ASSERT_EQ(res[2], 0);
    ASSERT_EQ(res[3], 0);

    ASSERT_EQ(res[4], 1920);
    ASSERT_EQ(res[5], 0);
    ASSERT_EQ(res[6], 1);
    ASSERT_EQ(res[7], 0);

    ASSERT_EQ(res[8], 1920);
    ASSERT_EQ(res[9], 1080);
    ASSERT_EQ(res[10], 1);
    ASSERT_EQ(res[11], 1);

    ASSERT_EQ(res[12], 0);
    ASSERT_EQ(res[13], 1080);
    ASSERT_EQ(res[14], 0);
    ASSERT_EQ(res[15], 1);
}

TEST(ImagePositionCalculator, ImageIsExactlyHalfOfDeviceResolutionScalesAndFitsExactly)
{
    ImagePositionCalculator imagePositionCalculator{};

    auto res = imagePositionCalculator.GetCenteredRectangleVertexCoordinates(1000, 1000, 500, 500);
    ASSERT_EQ(res[0], 0);
    ASSERT_EQ(res[1], 0);
    ASSERT_EQ(res[2], 0);
    ASSERT_EQ(res[3], 0);

    ASSERT_EQ(res[4], 1000);
    ASSERT_EQ(res[5], 0);
    ASSERT_EQ(res[6], 1);
    ASSERT_EQ(res[7], 0);

    ASSERT_EQ(res[8], 1000);
    ASSERT_EQ(res[9], 1000);
    ASSERT_EQ(res[10], 1);
    ASSERT_EQ(res[11], 1);

    ASSERT_EQ(res[12], 0);
    ASSERT_EQ(res[13], 1000);
    ASSERT_EQ(res[14], 0);
    ASSERT_EQ(res[15], 1);
}

TEST(ImagePositionCalculator, ImageIsExactlyDoubleOfDeviceResolutionScalesAndFitsExactly)
{
    ImagePositionCalculator imagePositionCalculator{};

    auto res = imagePositionCalculator.GetCenteredRectangleVertexCoordinates(1000, 1000, 2000, 2000);
    ASSERT_EQ(res[0], 0);
    ASSERT_EQ(res[1], 0);
    ASSERT_EQ(res[2], 0);
    ASSERT_EQ(res[3], 0);

    ASSERT_EQ(res[4], 1000);
    ASSERT_EQ(res[5], 0);
    ASSERT_EQ(res[6], 1);
    ASSERT_EQ(res[7], 0);

    ASSERT_EQ(res[8], 1000);
    ASSERT_EQ(res[9], 1000);
    ASSERT_EQ(res[10], 1);
    ASSERT_EQ(res[11], 1);

    ASSERT_EQ(res[12], 0);
    ASSERT_EQ(res[13], 1000);
    ASSERT_EQ(res[14], 0);
    ASSERT_EQ(res[15], 1);
}

TEST(ImagePositionCalculator, ImageIs4x3AndLargerGetsScaledDownCorrectly)
{
    ImagePositionCalculator imagePositionCalculator{};

    auto res = imagePositionCalculator.GetCenteredRectangleVertexCoordinates(100, 100, 640, 480);
    ASSERT_EQ(res[0], 0);
    ASSERT_EQ(res[1], 50 - 37); //100/640 = 37.5 ->(int)37.5 -> 37
    ASSERT_EQ(res[2], 0);
    ASSERT_EQ(res[3], 0);

    ASSERT_EQ(res[4], 100);
    ASSERT_EQ(res[5], 50 - 37);
    ASSERT_EQ(res[6], 1);
    ASSERT_EQ(res[7], 0);

    ASSERT_EQ(res[8], 100);
    ASSERT_EQ(res[9], 50 + 37);
    ASSERT_EQ(res[10], 1);
    ASSERT_EQ(res[11], 1);

    ASSERT_EQ(res[12], 0);
    ASSERT_EQ(res[13], 50 + 37);
    ASSERT_EQ(res[14], 0);
    ASSERT_EQ(res[15], 1);
}
