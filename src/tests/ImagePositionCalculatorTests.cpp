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
    ASSERT_EQ(res[0], 250);
    ASSERT_EQ(res[1], 250);
    ASSERT_EQ(res[2], 0);
    ASSERT_EQ(res[3], 0);

    ASSERT_EQ(res[4], 750);
    ASSERT_EQ(res[5], 250);
    ASSERT_EQ(res[6], 1);
    ASSERT_EQ(res[7], 0);

    ASSERT_EQ(res[8], 750);
    ASSERT_EQ(res[9], 750);
    ASSERT_EQ(res[10], 1);
    ASSERT_EQ(res[11], 1);

    ASSERT_EQ(res[12], 250);
    ASSERT_EQ(res[13], 750);
    ASSERT_EQ(res[14], 0);
    ASSERT_EQ(res[15], 1);
}

