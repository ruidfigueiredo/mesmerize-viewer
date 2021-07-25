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
#include <functional>

enum class PictureLoadingState
{
    EMPTY,
    SEND_TO_GPU,
    LOADED
};

enum class PictureScaleMode
{
    SCALE_TO_FIT,
    COVER,
    //ZOOM is COVER + scale 50%
    ZOOM
};

enum class PictureEffects
{
    NONE,
    GAUSSIAN_BLUR
};

struct PictureLoadResult {
    int TextureSlot;
    int Width;
    int Height;
    int BytesPerPixel;
    unsigned char *LoadedImage;
    std::array<float, 16> VertexCoordinates;
    PictureScaleMode pictureScaleMode;
    std::function<void()> FreeImage = nullptr;
    std::function<void()> OnLoaded = nullptr;
    std::string Path;

    virtual ~PictureLoadResult() {
        if (FreeImage != nullptr) {
            std::cout << "Freeing memory for image" << Path << std::endl;
            FreeImage();
        }
    }
};

class Picture
{
    unsigned int _mainTextureId;
    std::shared_ptr<ResolutionScaleCalculator> _resolutionScaleCalculator;
    std::shared_ptr<ImagePositionCalculator> _imagePositionCalculator;
    std::unique_ptr<VertexArray> _vertexArray;
    std::unique_ptr<VertexBuffer> _vertexBuffer;
    std::unique_ptr<IndexBuffer> _indexBuffer;
    PictureLoadingState _pictureLoadingState;
    std::shared_ptr<PictureLoadResult> _pictureLoadResult;
    std::thread _loadingThread;
    static std::mutex ImageLoadingMutex;
    static ShaderProgram PictureShaderProgram;

    void SetupVertexArray(const std::array<float, 16> & squareCoordinatesWithTextureCoordinates);
    std::pair<int, int> CalculateScaledDimensions(PictureScaleMode pictureScaleMode, int deviceWidth, int deviceHeight, int width, int height);
    void SendToGpu();
    void RenderPicture(glm::mat4 mvp, float opacity);
    void HandleSizeChanged(int newWidth, int newHeight);

public:
    Picture(std::shared_ptr<ResolutionScaleCalculator> rsc, std::shared_ptr<ImagePositionCalculator> imagePositionCalculator);
    Picture(Picture&& rhs);
    ~Picture();
    static void InitShaders();
    void Load(std::string pathToFile, int textureSlot, PictureScaleMode pictureScaleMode = PictureScaleMode::SCALE_TO_FIT, PictureEffects pictureLoadingMode = PictureEffects::NONE, const std::function<void()> onLoaded = nullptr);
    void Render(glm::mat4 mvp = glm::ortho(0.0f, (float)DeviceInformation::getWidth(), 0.0f, (float)DeviceInformation::getHeight(), -1.0f, 1.0f), float opacity = 1.0f);
};