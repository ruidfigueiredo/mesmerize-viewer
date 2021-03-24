#include "Picture.h"
#include "GL/glew.h"
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <iir_gauss_blur.h>
#include <iostream>
#include "ImagePositionCalculator.h"
#include "CheckGlErrors.h"
#include <thread>
#include <algorithm>
#include "VertexBuffer.h"
#include "DeviceInformation.h"

//statics
std::mutex Picture::ImageLoadingMutex;
ShaderProgram Picture::PictureShaderProgram;
void Picture::InitShaders()
{
    PictureShaderProgram.Init();
    PictureShaderProgram.AddVertexShader("shaders/vertex.shader");
    PictureShaderProgram.AddFragmentShader("shaders/fragment.shader");
}

Picture::Picture(std::shared_ptr<ResolutionScaleCalculator> rcs, std::shared_ptr<ImagePositionCalculator> imagePositionCalculator) : _resolutionScaleCalculator(rcs),
                                                                                                                                     _pictureLoadingState(PictureLoadingState::EMPTY),
                                                                                                                                     _imagePositionCalculator(imagePositionCalculator)
{
    GL_CALL(glGenTextures(1, &_mainTextureId));
    DeviceInformation::registerSizeChangedCallback([](int width, int height){
        std::cout << "Picture: (newWidth: " << width << ", newHeight: " << height << ")\n";
    });
}

Picture::~Picture()
{
    if (_loadingThread.joinable())
    {
        _loadingThread.join(); //let it finish if it needs to
    }
}

void Picture::Load(std::string path, int textureSlot, PictureSize pictureSize, PictureLoadingMode pictureLoadingMode, const std::function<void(bool)>& onLoaded)
{
    std::cout << "Loading new picture: " << path << " \n";
    if (_loadingThread.joinable())
    {
        std::cout << "\t\tAnother picture was loading, waiting for it to finish\n";
        _loadingThread.join(); //this will block the rendering thread but should only happen if the image takes so long to load that there's a new one to replace it that will need to wait for the first to finish
        std::cout << "\t\tContinuing loading: " << path << "\n";
    }


    _loadingThread = std::move(std::thread{[textureSlot, pictureSize, pictureLoadingMode, path, this] {
        std::lock_guard<std::mutex> imageLoadingLockGuard{ImageLoadingMutex};
        auto newPictureLoadResult = std::make_shared<PictureLoadResult>();
        newPictureLoadResult->TextureSlot = textureSlot;

        std::cout << "In memory picture loading thread running, loading " << path << "\n";
        newPictureLoadResult->Path = path;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *loadedImage = stbi_load(path.c_str(), &newPictureLoadResult->Width, &newPictureLoadResult->Height, &newPictureLoadResult->BytesPerPixel, 3);
        if (loadedImage == nullptr)
        {
            std::cout << "Failed to load image in path: " << path << std::endl;
            return;
        }
        if (_resolutionScaleCalculator->IsScallingRequired(newPictureLoadResult->Width, newPictureLoadResult->Height, DeviceInformation::getMaxTextureSize()))
        {
            std::cout << "Scaling is required (" << newPictureLoadResult->Width << " x " << newPictureLoadResult->Height << ")\n";
            auto scaledDimensions = _resolutionScaleCalculator->ScaleToMaxDimension(newPictureLoadResult->Width, newPictureLoadResult->Height, DeviceInformation::getMaxTextureSize());
            int newWidth = scaledDimensions.first;
            int newHeight = scaledDimensions.second;
            unsigned char *output_pixels = (unsigned char *)malloc(newWidth * newHeight * newPictureLoadResult->BytesPerPixel);
            stbir_resize_uint8(loadedImage, newPictureLoadResult->Width, newPictureLoadResult->Height, 0, output_pixels, newWidth, newHeight, 0, newPictureLoadResult->BytesPerPixel);
            stbi_image_free(loadedImage);
            newPictureLoadResult->LoadedImage = output_pixels;
            newPictureLoadResult->Width = newWidth;
            newPictureLoadResult->Height = newHeight;
            newPictureLoadResult->FreeImage = [output_pixels, path] {
                delete output_pixels;
                std::cout << "Resized picture " << path << " memory freed\n";
            };
        }
        else
        {
            newPictureLoadResult->LoadedImage = loadedImage;
            newPictureLoadResult->FreeImage = [loadedImage, path] {
                stbi_image_free(loadedImage);
                std::cout << "Picture " << path << " memory freed\n";
            };
        }

        if (pictureLoadingMode == PictureLoadingMode::GAUSSIAN_BLUR)
        {
            float sigma = 10.0f; //produces a nice result, see http://arkanis.de/weblog/2018-08-30-iir-gauss-blur-h-a-gaussian-blur-single-header-file-library
            iir_gauss_blur(newPictureLoadResult->Width, newPictureLoadResult->Height, newPictureLoadResult->BytesPerPixel, newPictureLoadResult->LoadedImage, sigma);
        }
        else if (pictureLoadingMode != PictureLoadingMode::NORMAL)
        {
            throw std::range_error("Unknown value for PictureLoadingMode enum: " + std::to_string((int)pictureLoadingMode));
        }

        std::pair<int, int> finalDimensions;
        if (pictureSize == PictureSize::COVER)
        {
            finalDimensions = _resolutionScaleCalculator->ScaleToCover(DeviceInformation::getWidth(), DeviceInformation::getHeight(), newPictureLoadResult->Width, newPictureLoadResult->Height);
        }
        else if (pictureSize == PictureSize::SCALE_TO_FIT)
        {
            finalDimensions = _resolutionScaleCalculator->ScaleToFit(DeviceInformation::getWidth(), DeviceInformation::getHeight(), newPictureLoadResult->Width, newPictureLoadResult->Height);
        }
        else if (pictureSize == PictureSize::ZOOM)
        {
            finalDimensions = _resolutionScaleCalculator->ScaleToCover(DeviceInformation::getWidth(), DeviceInformation::getHeight(), newPictureLoadResult->Width, newPictureLoadResult->Height);
            finalDimensions.first *= 1.5;
            finalDimensions.second *= 1.5;
        }
        else
        {
            throw std::range_error("Unknown value for PictureSize enum: " + std::to_string((int)pictureSize));
        }
        newPictureLoadResult->VertexCoordinates = _imagePositionCalculator->GetCenteredRectangleVertexCoordinates(DeviceInformation::getWidth(), DeviceInformation::getHeight(), finalDimensions.first, finalDimensions.second);
        _pictureLoadResult = newPictureLoadResult;
        _pictureLoadingState = PictureLoadingState::SEND_TO_GPU;
    }});
    std::cout << "Picture " << path << " loaded into memory\n";
}


void Picture::Render(glm::mat4 mvp, float opacity)
{
    if (_pictureLoadingState == PictureLoadingState::SEND_TO_GPU)
    {
        SendToGpu();
    }
    else if (_pictureLoadingState == PictureLoadingState::LOADED)
    {
        RenderPicture(mvp, opacity);
    }
}

void Picture::SendToGpu()
{
    std::shared_ptr<PictureLoadResult> pictureLoadResult;
    {
        std::lock_guard<std::mutex> imageLoadingLockGuard{ImageLoadingMutex};
        pictureLoadResult = _pictureLoadResult;
    }

    auto start = std::chrono::steady_clock::now();
    std::cout << "SEND_TO_GPU:" << pictureLoadResult->Path << "\n";
    GL_CALL(glActiveTexture(GL_TEXTURE0 + pictureLoadResult->TextureSlot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _mainTextureId));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, pictureLoadResult->Width, pictureLoadResult->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, pictureLoadResult->LoadedImage));

    _vertexArray.reset();
    _vertexBuffer.reset();
    _indexBuffer.reset();
    _vertexArray = std::make_shared<VertexArray>();
    _vertexBuffer = std::make_shared<VertexBuffer>(&_pictureLoadResult->VertexCoordinates, sizeof(_pictureLoadResult->VertexCoordinates));
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

void Picture::RenderPicture(glm::mat4 mvp, float opacity)
{
    PictureShaderProgram.Bind();
    PictureShaderProgram.SetUniformf("blendValue", opacity);
    PictureShaderProgram.SetUniformMat4f("mvp", mvp);
    PictureShaderProgram.SetUniformi("textureSlot", _pictureLoadResult->TextureSlot);

    GL_CALL(glActiveTexture(GL_TEXTURE0 + _pictureLoadResult->TextureSlot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _mainTextureId));
    _vertexArray->Bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    _vertexArray->Unbind();
    glBindTexture(GL_TEXTURE_2D, 0);
    PictureShaderProgram.Unbind();
}

