#include "Picture.h"
#include "GL/glew.h"
#include <stb/stb_image.h>
#include <stb/stb_image_resize.h>
#include <iir_gauss_blur.h>
#include "ImagePositionCalculator.h"
#include "CheckGlErrors.h"
#include <thread>
#include <algorithm>
#include "VertexBuffer.h"
#include "DeviceInformation.h"

#include <iostream>


#ifdef TRACE_PICTURE
#include <iostream>
    #define DEBUG_PICTURE(fn) \
            fn
#else
    #define DEBUG_PICTURE(fn)
#endif


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
    DeviceInformation::registerSizeChangedCallback([this](int width, int height){
        HandleSizeChanged(width, height);
    }, this);
}

Picture::Picture(Picture &&rhs) :
    _mainTextureId(std::move(rhs._mainTextureId)),
    _resolutionScaleCalculator(std::move(rhs._resolutionScaleCalculator)),
    _imagePositionCalculator(std::move(rhs._imagePositionCalculator)),
    _vertexArray(std::move(rhs._vertexArray)),
    _vertexBuffer(std::move(rhs._vertexBuffer)),
    _indexBuffer(std::move(rhs._indexBuffer)),
    _pictureLoadingState(std::move(rhs._pictureLoadingState)),
    _pictureLoadResult(std::move(rhs._pictureLoadResult)),
    _loadingThread(std::move(rhs._loadingThread))
    {}

Picture::~Picture()
{
    DeviceInformation::unRegisterSizeChangedCallback(this);
    if (_loadingThread.joinable())
    {
        _loadingThread.join(); //let it finish if it needs to
    }
}

void Picture::HandleSizeChanged(int newWidth, int newHeight)
{
    if (_pictureLoadResult == nullptr) return;

    {
        std::lock_guard<std::mutex> imageLoadingLockGuard{ImageLoadingMutex};

        auto finalDimensions = CalculateScaledDimensions(_pictureLoadResult->pictureScaleMode, newWidth, newHeight, _pictureLoadResult->Width, _pictureLoadResult->Height);
        _pictureLoadResult->VertexCoordinates = _imagePositionCalculator->GetCenteredRectangleVertexCoordinates(newWidth, newHeight, finalDimensions.first, finalDimensions.second);

        SetupVertexArray(_pictureLoadResult->VertexCoordinates);
    }
}

std::pair<int, int> Picture::CalculateScaledDimensions(PictureScaleMode pictureScaleMode, int deviceWidth, int deviceHeight, int width, int height)
{
    std::pair<int, int> finalDimensions;
    if (pictureScaleMode == PictureScaleMode::COVER)
    {
        finalDimensions = _resolutionScaleCalculator->ScaleToCover(deviceWidth, deviceHeight, width, height);
    }
    else if (pictureScaleMode == PictureScaleMode::SCALE_TO_FIT)
    {
        finalDimensions = _resolutionScaleCalculator->ScaleToFit(deviceWidth, deviceHeight, width, height);
    }
    else if (pictureScaleMode == PictureScaleMode::ZOOM)
    {
        finalDimensions = _resolutionScaleCalculator->ScaleToCover(deviceWidth, deviceHeight, width, height);
        finalDimensions.first *= 1.5;
        finalDimensions.second *= 1.5;
    }
    else {
        throw std::range_error("Unknown value for PictureScaleMode enum: " + std::to_string((int) pictureScaleMode));
    }
    return finalDimensions;
}

void Picture::Load(std::string path, int textureSlot, PictureScaleMode pictureScaleMode, PictureEffects pictureLoadingMode, const std::function<void()> onLoaded)
{

    DEBUG_PICTURE(std::cout << "Loading new picture: " << path << " \n");

    if (_loadingThread.joinable())
    {
        DEBUG_PICTURE(std::cout << "\t\tAnother picture was loading, waiting for it to finish\n";)
        _loadingThread.join(); //this will block the rendering thread but should only happen if the image takes so long to load that there's a new one to replace it that will need to wait for the first to finish
        DEBUG_PICTURE(std::cout << "\t\tContinuing loading: " << path << "\n");
    }


    _loadingThread = std::move(std::thread{[textureSlot, pictureScaleMode, pictureLoadingMode, path, this, onLoaded] {
        std::lock_guard<std::mutex> imageLoadingLockGuard{ImageLoadingMutex};
        auto newPictureLoadResult = std::make_shared<PictureLoadResult>();
        newPictureLoadResult->TextureSlot = textureSlot;
        newPictureLoadResult->pictureScaleMode = pictureScaleMode;
        newPictureLoadResult->OnLoaded = onLoaded;

        DEBUG_PICTURE(std::cout << "In memory picture loading thread running, loading " << path << "\n");
        newPictureLoadResult->Path = path;
        stbi_set_flip_vertically_on_load(true);
        unsigned char *loadedImage = stbi_load(path.c_str(), &newPictureLoadResult->Width, &newPictureLoadResult->Height, &newPictureLoadResult->BytesPerPixel, 3);
        if (loadedImage == nullptr)
        {
            DEBUG_PICTURE(std::cout << "Failed to load image in path: " << path << std::endl);
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
                DEBUG_PICTURE(std::cout << "Resized picture " << path << " memory freed\n");
            };
        }
        else
        {
            newPictureLoadResult->LoadedImage = loadedImage;
            newPictureLoadResult->FreeImage = [loadedImage, path] {
                stbi_image_free(loadedImage);
                DEBUG_PICTURE(std::cout << "Picture " << path << " memory freed\n");
            };
        }

        if (pictureLoadingMode == PictureEffects::GAUSSIAN_BLUR)
        {
            float sigma = 10.0f; //produces a nice result, see http://arkanis.de/weblog/2018-08-30-iir-gauss-blur-h-a-gaussian-blur-single-header-file-library
            iir_gauss_blur(newPictureLoadResult->Width, newPictureLoadResult->Height, newPictureLoadResult->BytesPerPixel, newPictureLoadResult->LoadedImage, sigma);
        }
        else if (pictureLoadingMode != PictureEffects::NONE)
        {
            throw std::range_error("Unknown value for PictureEffects enum: " + std::to_string((int)pictureLoadingMode));
        }

        std::pair<int, int> finalDimensions = CalculateScaledDimensions(pictureScaleMode, DeviceInformation::getWidth(), DeviceInformation::getHeight(), newPictureLoadResult->Width,
                                                                        newPictureLoadResult->Height);

        newPictureLoadResult->VertexCoordinates = _imagePositionCalculator->GetCenteredRectangleVertexCoordinates(DeviceInformation::getWidth(), DeviceInformation::getHeight(), finalDimensions.first, finalDimensions.second);
        _pictureLoadResult = newPictureLoadResult;
        _pictureLoadingState = PictureLoadingState::SEND_TO_GPU;
        DEBUG_PICTURE(std::cout << "Picture " << path << " loaded into memory\n");
        //if (onLoaded){
            DEBUG_PICTURE(std::cout << "Calling on loaded\n");
            //onLoaded();
        //}
        std::cout << "Picture " << path <<  " loaded into memory\n";
    }});
}


void Picture::Render(glm::mat4 mvp, float opacity)
{
    if (_pictureLoadingState == PictureLoadingState::SEND_TO_GPU)
    {
        std::cout << "Sending picture " << _pictureLoadResult->Path <<  "to vram\n";
        SendToGpu();
        std::cout << "Picture " << _pictureLoadResult->Path <<  " sent to vram\n";
    }
    else if (_pictureLoadingState == PictureLoadingState::LOADED)
    {
        RenderPicture(mvp, opacity);
    }
}
void Picture::SetupVertexArray(const std::array<float, 16> & squareCoordinatesWithTextureCoordinates) {
    _vertexArray.reset();
    _vertexBuffer.reset();
    _indexBuffer.reset();
    _vertexArray = std::make_unique<VertexArray>();
    _vertexBuffer = std::make_unique<VertexBuffer>(&squareCoordinatesWithTextureCoordinates, sizeof(squareCoordinatesWithTextureCoordinates));
    VertexBufferLayout twoFloatVertexCoordAndTwoFloatTextureCoordBufferLayout;
    twoFloatVertexCoordAndTwoFloatTextureCoordBufferLayout.Push<float>(2);
    twoFloatVertexCoordAndTwoFloatTextureCoordBufferLayout.Push<float>(2);
    _vertexArray->AddBuffer(*_vertexBuffer, twoFloatVertexCoordAndTwoFloatTextureCoordBufferLayout);
    uint indexes[] = {
            0, 1, 2,
            2, 3, 0};
    _indexBuffer = std::make_unique<IndexBuffer>(indexes, 6);
    _vertexArray->AddBuffer(*_indexBuffer);
}

void Picture::SendToGpu()
{
    std::shared_ptr<PictureLoadResult> pictureLoadResult;
    {
        std::lock_guard<std::mutex> imageLoadingLockGuard{ImageLoadingMutex};
        pictureLoadResult = _pictureLoadResult;
    }

    auto start = std::chrono::steady_clock::now();
    DEBUG_PICTURE(std::cout << "SEND_TO_GPU:" << pictureLoadResult->Path << "\n");
    GL_CALL(glActiveTexture(GL_TEXTURE0 + pictureLoadResult->TextureSlot));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _mainTextureId));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, pictureLoadResult->Width, pictureLoadResult->Height, 0, GL_RGB, GL_UNSIGNED_BYTE, pictureLoadResult->LoadedImage));

    SetupVertexArray(pictureLoadResult->VertexCoordinates);

    _pictureLoadingState = PictureLoadingState::LOADED;
    DEBUG_PICTURE(std::cout << "LOADED!\n");
    if (pictureLoadResult->OnLoaded){
        pictureLoadResult->OnLoaded();
    }


    auto end = std::chrono::steady_clock::now();
    DEBUG_PICTURE(std::cout << "Elapsed time in milliseconds : "
                   << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                   << " ms" << std::endl);
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
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
    PictureShaderProgram.Unbind();
}
