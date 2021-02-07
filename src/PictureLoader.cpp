#include "PictureLoader.h"
#include "GL/glew.h"
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <iostream>

PictureLoader::PictureLoader(std::shared_ptr<ResolutionScaleCalculator> rcs, std::shared_ptr<ImagePositionCalculator> ipc) 
    : _resolutionScaleCalculator(rcs), _imagePositionCalculator(ipc)
{    
    glGenBuffers(1, &_arrayBufferId);
    glGenBuffers(1, &_indexBufferId);
    glGenVertexArrays(1, &_vertexArrayId);
    unsigned int textureIds[2];
    glGenTextures(2, textureIds);
    _mainTextureId = textureIds[0];
    _blurryBackgroundTextureId = textureIds[1];    

    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxDimension);    
    _textureSlot = 0;
}


PictureLoadResult PictureLoader::Load(std::string path)
{
    stbi_set_flip_vertically_on_load(true);
    int width, height, bytesPerPixel;
    unsigned char *loadedImage = stbi_load(path.c_str(), &width, &height, &bytesPerPixel, 3);
    if (loadedImage == nullptr)
    {
        return PictureLoadResult{
            false,
            std::array<int, 8>{}};
    }

    glActiveTexture(GL_TEXTURE0 + _textureSlot);
    glBindTexture(GL_TEXTURE_2D, _mainTextureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    

    if (_resolutionScaleCalculator->IsScallingRequired(width, height, _maxDimension))
    {
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
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, loadedImage);
    }

    auto coordinates = _imagePositionCalculator->GetCenteredRectangleVertexCoordinates(1920, 1080, width, height);
    glBindVertexArray(_vertexArrayId);
    glBindBuffer(GL_ARRAY_BUFFER, _arrayBufferId);    
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*coordinates.size(), &coordinates[0], GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    glEnableVertexAttribArray(1);
    unsigned long offset = 2 * sizeof(float);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (const void *)offset) ;
    uint indexes[] = {
        0, 1, 2,
        2, 3, 0};
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(unsigned int), indexes, GL_STATIC_DRAW);
    
    //missing glActivateTexture(GL_TEXTURE0) with blurry version
    //glBindTexture(GL_TEXTURE_2D, _blurryBackgroundTextureId)
    stbi_image_free(loadedImage);    
}

void PictureLoader::Render() {
    glActiveTexture(GL_TEXTURE0 + _textureSlot);
    glBindTexture(GL_TEXTURE_2D, _mainTextureId);        
    glBindVertexArray(_vertexArrayId);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
