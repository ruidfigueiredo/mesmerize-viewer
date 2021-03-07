#pragma once
#include <array>
#include <memory>
#include "ResolutionScaleCalculator.h"
#include "VertexArray.h"
#include "ImagePositionCalculator.h"
#include <thread>
#include <functional>
#include <mutex>

enum PictureLoadingState
{
    EMPTY,
    SEND_TO_GPU,
    LOADED
};

struct PictureLoadResult
{
    int TextureSlot;
    int Width;
    int Height;
    int BytesPerPixel;
    unsigned char *LoadedImage;
    std::array<float, 16> VertexCoordinates;
    std::function<void()> FreeImage;
};

class Picture
{
    unsigned int _mainTextureId;
    unsigned int _blurryBackgroundTextureId;
    std::shared_ptr<ResolutionScaleCalculator> _resolutionScaleCalculator;
    std::shared_ptr<ImagePositionCalculator> _imagePositionCalculator;
    std::shared_ptr<VertexArray> _vertexArray;
    std::shared_ptr<VertexBuffer> _vertexBuffer;
    std::shared_ptr<IndexBuffer> _indexBuffer;
    int _maxDimension;
    PictureLoadingState _pictureLoadingState;
    PictureLoadResult _pictureLoadResult;
    std::thread _loadingThread;
    int _maxDeviceWidth;
    int _maxDeviceHeight;
    int _activeTextureSlot;
    std::mutex _imageLoadingMutex;

public:
    Picture(std::shared_ptr<ResolutionScaleCalculator> rsc, std::shared_ptr<ImagePositionCalculator> imagePositionCalculator);
    ~Picture();
    void Load(std::string pathToFile, int textureSlot);
    void Render();
};