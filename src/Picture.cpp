#include "Picture.h"
#include "GL/glew.h"
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <iostream>
#include "ImagePositionCalculator.h"

Picture::Picture(std::shared_ptr<ResolutionScaleCalculator> rcs) : _resolutionScaleCalculator(rcs)
{
    unsigned int textureIds[2];
    glGenTextures(2, textureIds);
    _mainTextureId = textureIds[0];
    _blurryBackgroundTextureId = textureIds[1];

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxDimension);
}

PictureLoadResult Picture::Load(std::string path, int textureSlot)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, bytesPerPixel;
    unsigned char *loadedImage = stbi_load(path.c_str(), &width, &height, &bytesPerPixel, 3);
    if (loadedImage == nullptr)
    {
        return PictureLoadResult{
            false,
            std::array<float, 16>{}};
    }

    glActiveTexture(GL_TEXTURE0 + textureSlot);
    glBindTexture(GL_TEXTURE_2D, _mainTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    if (_resolutionScaleCalculator->IsScallingRequired(width, height, _maxDimension))
    {
        std::cout << "Scalling is required\n";
        auto scaledDimensions = _resolutionScaleCalculator->ScaleToMaxDimension(width, height, _maxDimension);
        int newWidth = scaledDimensions.first;
        int newHeight = scaledDimensions.second;
        unsigned char *output_pixels = (unsigned char *)malloc(newWidth * newHeight * bytesPerPixel);
        stbir_resize_uint8(loadedImage, width, height, 0, output_pixels, newWidth, newHeight, 0, bytesPerPixel);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, newWidth, newHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, output_pixels);
        delete output_pixels;
    }
    else
    {
        std::cout << "Scalling isn't required\n";
        std::cout << "*************************************\n";
        std::cout << "Image dimensions: (" << width << ", " << height << ")\n";
        std::cout << "*************************************\n";

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, loadedImage);
        std::cout << "**********LOADED***********************\n";
    }

    //missing glActivateTexture(GL_TEXTURE0) with blurry version
    //glBindTexture(GL_TEXTURE_2D, _blurryBackgroundTextureId)
    ImagePositionCalculator ipc{_resolutionScaleCalculator};
    stbi_image_free(loadedImage);
    return PictureLoadResult{
        true,
        ipc.GetCenteredRectangleVertexCoordinates(1920, 1080, width, height)};
}
