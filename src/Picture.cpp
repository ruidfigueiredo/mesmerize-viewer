#include "Picture.h"
#include "GL/glew.h"
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <iostream>
#include "ImagePositionCalculator.h"
#include "CheckGlErrors.h"
#include <thread>
#include <algorithm>
#include "VertexBuffer.h"

Picture::Picture(std::shared_ptr<ResolutionScaleCalculator> rcs, std::shared_ptr<ImagePositionCalculator> imagePositionCalculator) : _resolutionScaleCalculator(rcs),
                                                                                                                                     _pictureLoadingState(PictureLoadingState::EMPTY),
                                                                                                                                     _activeTextureSlot(0),
                                                                                                                                     _maxDeviceWidth(3840),
                                                                                                                                     _maxDeviceHeight(2160),
                                                                                                                                     _imagePositionCalculator(imagePositionCalculator)
{
    unsigned int textureIds[2];
    GL_CALL(glGenTextures(2, textureIds));
    _mainTextureId = textureIds[0];
    _blurryBackgroundTextureId = textureIds[1];

    GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxDimension));
    _maxDimension = std::min(_maxDimension, 3960); //4K max, if possible
}

Picture::~Picture()
{
    if (_loadingThread.joinable())
    {
        _loadingThread.join(); //let it finish if it needs to
    }
}

void Picture::Load(std::string path, int textureSlot)
{
    if (_loadingThread.joinable())
    {
        _loadingThread.join(); //this will block the rendering thread but should only happen if the image takes so long to load that there's a new one to replace it that will need to wait for the first to finish
    }

    _pictureLoadResult = {};
    _pictureLoadResult.TextureSlot = textureSlot;

    std::thread loadThead([path, this] {
        std::cout << "Thread running, loading " << path << "\n";
        stbi_set_flip_vertically_on_load(true);
        unsigned char *loadedImage = stbi_load(path.c_str(), &_pictureLoadResult.Width, &_pictureLoadResult.Height, &_pictureLoadResult.BytesPerPixel, 3);
        if (loadedImage == nullptr)
        {
            std::cout << "Failed to load image in path: " << path << std::endl;
            return;
        }
        if (_resolutionScaleCalculator->IsScallingRequired(_pictureLoadResult.Width, _pictureLoadResult.Height, _maxDimension))
        {
            std::cout << "Scaling is required (" << _pictureLoadResult.Width << " x " << _pictureLoadResult.Height << ")\n";
            auto scaledDimensions = _resolutionScaleCalculator->ScaleToMaxDimension(_pictureLoadResult.Width, _pictureLoadResult.Height, _maxDimension);
            int newWidth = scaledDimensions.first;
            int newHeight = scaledDimensions.second;
            unsigned char *output_pixels = (unsigned char *)malloc(newWidth * newHeight * _pictureLoadResult.BytesPerPixel);
            stbir_resize_uint8(loadedImage, _pictureLoadResult.Width, _pictureLoadResult.Height, 0, output_pixels, newWidth, newHeight, 0, _pictureLoadResult.BytesPerPixel);
            stbi_image_free(loadedImage);
            _pictureLoadResult.LoadedImage = output_pixels;
            _pictureLoadResult.Width = newWidth;
            _pictureLoadResult.Height = newHeight;
            _pictureLoadResult.FreeImage = [this]() {
                delete _pictureLoadResult.LoadedImage;
                _pictureLoadResult.LoadedImage = nullptr;
            };
        }
        else
        {
            _pictureLoadResult.LoadedImage = loadedImage;
            _pictureLoadResult.FreeImage = [this]() {
                stbi_image_free(_pictureLoadResult.LoadedImage);
                _pictureLoadResult.LoadedImage = nullptr;
            };
        }
        auto scaleToCover = _resolutionScaleCalculator->ScaleToFit(3840, 2160, _pictureLoadResult.Width, _pictureLoadResult.Height);
        std::cout << "Scale to fit results: " << scaleToCover.first << "x" << scaleToCover.second << "\n";
        _pictureLoadResult.VertexCoordinates = _imagePositionCalculator->GetCenteredRectangleVertexCoordinates(_maxDeviceWidth, _maxDeviceHeight, scaleToCover.first, scaleToCover.second);
        _pictureLoadingState = PictureLoadingState::SEND_TO_GPU;
    });

    loadThead.detach();
    _loadingThread = std::move(loadThead);
}

void Picture::Render()
{
    if (_pictureLoadingState == PictureLoadingState::SEND_TO_GPU)
    {
        std::cout << "SEND_TO_GPU!\n";
        GL_CALL(glActiveTexture(GL_TEXTURE0 + _pictureLoadResult.TextureSlot));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _mainTextureId));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _pictureLoadResult.Width, _pictureLoadResult.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, _pictureLoadResult.LoadedImage));
        _pictureLoadResult.FreeImage();

        _vertexArray.reset();
        _vertexBuffer.reset();
        _indexBuffer.reset();
        _vertexArray = std::make_shared<VertexArray>();
        _vertexBuffer = std::make_shared<VertexBuffer>(&_pictureLoadResult.VertexCoordinates, sizeof(_pictureLoadResult.VertexCoordinates));
        VertexBufferLayout twoFloatVertexCoordAndTwoFloatTextureCoordBufferLayout;
        twoFloatVertexCoordAndTwoFloatTextureCoordBufferLayout.Push<float>(2);
        twoFloatVertexCoordAndTwoFloatTextureCoordBufferLayout.Push<float>(2);
        _vertexArray->AddBuffer(*_vertexBuffer, twoFloatVertexCoordAndTwoFloatTextureCoordBufferLayout);
        uint indexes[] = {
            0, 1, 2,
            2, 3, 0};
        _indexBuffer = std::make_shared<IndexBuffer>(indexes, 6);
        _vertexArray->AddBuffer(*_indexBuffer);

        _pictureLoadingState = PictureLoadingState::LOADED;
        std::cout << "LOADED!\n";
    }
    else if (_pictureLoadingState == PictureLoadingState::LOADED)
    {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + _pictureLoadResult.TextureSlot));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, _mainTextureId));
        _vertexArray->Bind();
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        _vertexArray->Unbind();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}