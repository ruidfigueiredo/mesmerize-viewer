#include <algorithm>
#include "ImagePositionCalculator.h"
#include <iostream>

std::array<float, 16> ImagePositionCalculator::GetCenteredRectangleVertexCoordinates(int deviceWidth, int deviceHeight, int imageWidth, int imageHeight)
{
    auto newDimensions = _resolutionScaleCalculator->ScaleToFit(deviceWidth, deviceHeight, imageWidth, imageHeight);
    const int width = newDimensions.first;
    const int height = newDimensions.second;

    std::cout << "++++++++++++++++++++++++++++++\n";
    std::cout << "Scalled image dimensions: (" << width << ", " << height << ")\n";
    std::cout << "++++++++++++++++++++++++++++++\n";

    //assumes we are using an elements array to build the 2 triangles that make up the square
    //format is: x, y, s, t for each point [s, t are the normalized texture coordinates]
    return std::array<float, 16>{
        1.0f * deviceWidth / 2 - width / 2,
        1.0f * deviceHeight / 2 - height / 2, 0.0f, 0.0f,
        1.0f * deviceWidth / 2 + width / 2,
        1.0f * deviceHeight / 2 - height / 2, 1.0f, 0.0f,
        1.0f * deviceWidth / 2 + width / 2,
        1.0f * deviceHeight / 2 + height / 2, 1.0f, 1.0f,
        1.0f * deviceWidth / 2 - width / 2,
        1.0f * deviceHeight / 2 + height / 2, 0.0f, 1.0f};
}