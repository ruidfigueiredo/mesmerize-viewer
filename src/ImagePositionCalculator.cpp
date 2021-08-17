#include <algorithm>
#include "ImagePositionCalculator.h"
#include <iostream>

std::array<float, 16> ImagePositionCalculator::GetCenteredRectangleVertexCoordinates(int deviceWidth, int deviceHeight, int imageWidth, int imageHeight)
{
    //assumes we are using an elements array to build the 2 triangles that make up the square
    //format is: x, y, s, t for each point [s, t are the normalized texture coordinates]
    return std::array<float, 16>{
        1.0f * deviceWidth / 2 - imageWidth / 2,
        1.0f * deviceHeight / 2 - imageHeight / 2, 0.0f, 0.0f,
        1.0f * deviceWidth / 2 + imageWidth / 2,
        1.0f * deviceHeight / 2 - imageHeight / 2, 1.0f, 0.0f,
        1.0f * deviceWidth / 2 + imageWidth / 2,
        1.0f * deviceHeight / 2 + imageHeight / 2, 1.0f, 1.0f,
        1.0f * deviceWidth / 2 - imageWidth / 2,
        1.0f * deviceHeight / 2 + imageHeight / 2, 0.0f, 1.0f};
}