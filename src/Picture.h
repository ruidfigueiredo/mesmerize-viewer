#pragma once
#include <array>
#include <memory>
#include "ResolutionScaleCalculator.h"
#include "VertexArray.h"
#include "ImagePositionCalculator.h"
#include "ShaderProgram.h"
#include <thread>
#include <functional>
#include <mutex>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "DeviceInformation.h"

enum class PictureLoadingState
{
    EMPTY,
    SEND_TO_GPU,
    LOADED
};

enum class PictureSize
{
    SCALE_TO_FIT,
    COVER,
    //ZOOM is COVER + scale 50%
    ZOOM
};

enum class PictureLoadingMode
{
    NORMAL,
    GAUSSIAN_BLUR
};

struct PictureLoadResult {
    int TextureSlot;
    int Width;
    int Height;
    int BytesPerPixel;
    unsigned char *LoadedImage;
    std::array<float, 16> VertexCoordinates;
    std::function<void()> FreeImage;
    std::string Path;
};

class Picture
{
    unsigned int _mainTextureId;
    std::shared_ptr<ResolutionScaleCalculator> _resolutionScaleCalculator;
    std::shared_ptr<ImagePositionCalculator> _imagePositionCalculator;
    std::shared_ptr<VertexArray> _vertexArray;
    std::shared_ptr<VertexBuffer> _vertexBuffer;
    std::shared_ptr<IndexBuffer> _indexBuffer;
    PictureLoadingState _pictureLoadingState;
    PictureLoadResult _pictureLoadResult;
    std::thread _loadingThread;
    static std::mutex ImageLoadingMutex;
    static ShaderProgram PictureShaderProgram;

    void SendToGpu();
    void RenderPicture(glm::mat4 mvp, float opacity);

public:
    Picture(std::shared_ptr<ResolutionScaleCalculator> rsc, std::shared_ptr<ImagePositionCalculator> imagePositionCalculator);
    ~Picture();
    static void InitShaders();
    void Load(std::string pathToFile, int textureSlot, PictureSize size = PictureSize::SCALE_TO_FIT, PictureLoadingMode pictureLoadingMode = PictureLoadingMode::NORMAL);
    void Render(glm::mat4 mvp = glm::ortho(0.0f, (float)DeviceInformation::getWidth(), 0.0f, (float)DeviceInformation::getHeight(), -1.0f, 1.0f), float opacity = 1.0f);
};