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
#include "DeviceInformation.h"

Picture::Picture(std::shared_ptr<ResolutionScaleCalculator> rcs, std::shared_ptr<ImagePositionCalculator> imagePositionCalculator) : _resolutionScaleCalculator(rcs),
                                                                                                                                     _pictureLoadingState(PictureLoadingState::EMPTY),
                                                                                                                                     _activeTextureSlot(0),
                                                                                                                                     _imagePositionCalculator(imagePositionCalculator)
{
    unsigned int textureIds[2];
    GL_CALL(glGenTextures(2, textureIds));
    _mainTextureId = textureIds[0];
    _blurryBackgroundTextureId = textureIds[1];
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
    std::cout << "----------------------> LOAD START: " << path << " \n";
    if (_loadingThread.joinable())
    {
        std::cout << "\t\tCALLING JOIN, EXPECTING THAT THE THREAD WILL BLOCK!!!\n";
        _loadingThread.join(); //this will block the rendering thread but should only happen if the image takes so long to load that there's a new one to replace it that will need to wait for the first to finish
        std::cout << "\t\t\tJOINED!!!\n";
    }

    _pictureLoadResult = {};
    _pictureLoadResult.TextureSlot = textureSlot;

    _loadingThread = std::move(std::thread{[path, this] {
        std::lock_guard<std::mutex> imageLoadingLockGuard{_imageLoadingMutex};

        std::cout << "Thread running, loading " << path << "\n";
        _pictureLoadResult.Path = path;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *loadedImage = stbi_load(path.c_str(), &_pictureLoadResult.Width, &_pictureLoadResult.Height, &_pictureLoadResult.BytesPerPixel, 3);
        if (loadedImage == nullptr)
        {
            std::cout << "Failed to load image in path: " << path << std::endl;
            return;
        }
        if (_resolutionScaleCalculator->IsScallingRequired(_pictureLoadResult.Width, _pictureLoadResult.Height, DeviceInformation::getMaxTextureSize()))
        {
            std::cout << "Scaling is required (" << _pictureLoadResult.Width << " x " << _pictureLoadResult.Height << ")\n";
            auto scaledDimensions = _resolutionScaleCalculator->ScaleToMaxDimension(_pictureLoadResult.Width, _pictureLoadResult.Height, DeviceInformation::getMaxTextureSize());
            int newWidth = scaledDimensions.first;
            int newHeight = scaledDimensions.second;
            unsigned char *output_pixels = (unsigned char *)malloc(newWidth * newHeight * _pictureLoadResult.BytesPerPixel);
            stbir_resize_uint8(loadedImage, _pictureLoadResult.Width, _pictureLoadResult.Height, 0, output_pixels, newWidth, newHeight, 0, _pictureLoadResult.BytesPerPixel);
            stbi_image_free(loadedImage);
            _pictureLoadResult.LoadedImage = output_pixels;
            _pictureLoadResult.Width = newWidth;
            _pictureLoadResult.Height = newHeight;
            _pictureLoadResult.FreeImage = [this] {
                delete _pictureLoadResult.LoadedImage;
                _pictureLoadResult.LoadedImage = nullptr;
            };
        }
        else
        {
            _pictureLoadResult.LoadedImage = loadedImage;
            _pictureLoadResult.FreeImage = [this] {
                stbi_image_free(_pictureLoadResult.LoadedImage);
                _pictureLoadResult.LoadedImage = nullptr;
            };
        }
        auto scaledDimentions = _resolutionScaleCalculator->ScaleToFit(DeviceInformation::getWidth(), DeviceInformation::getHeight(), _pictureLoadResult.Width, _pictureLoadResult.Height);
        _pictureLoadResult.VertexCoordinates = _imagePositionCalculator->GetCenteredRectangleVertexCoordinates(DeviceInformation::getWidth(), DeviceInformation::getHeight(), scaledDimentions.first, scaledDimentions.second);
        _pictureLoadingState = PictureLoadingState::SEND_TO_GPU;
    }});
    std::cout << "----------------------> LOAD END\n";
}

void Picture::Render()
{
    if (_pictureLoadingState == PictureLoadingState::SEND_TO_GPU)
    {
        SendToGpu();
    }
    else if (_pictureLoadingState == PictureLoadingState::LOADED)
    {
        RenderPicture();
    }
}

void Picture::SendToGpu()
{

    auto start = std::chrono::steady_clock::now();
    std::cout << "SEND_TO_GPU:" << _pictureLoadResult.Path << "\n";
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _pictureLoadResult.TextureSlot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _mainTextureId));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    {
        std::lock_guard<std::mutex> imageLoadingLockGuard{_imageLoadingMutex};
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, _pictureLoadResult.Width, _pictureLoadResult.Height, 0, GL_RGB, GL_UNSIGNED_BYTE, _pictureLoadResult.LoadedImage));
    }
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

    std::cout << "++++++++++++++ RELEASED image loading mutex ++++++++++++++*************\n";
    auto end = std::chrono::steady_clock::now();
    std::cout << "Elapsed time in milliseconds : "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms" << std::endl;
}

void Picture::RenderPicture()
{
    GL_CALL(glActiveTexture(GL_TEXTURE0 + _pictureLoadResult.TextureSlot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _mainTextureId));
    _vertexArray->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    _vertexArray->Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
}