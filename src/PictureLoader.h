#pragma once
#include <array>
#include <memory>
#include "ResolutionScaleCalculator.h"
#include "ImagePositionCalculator.h"

struct PictureLoadResult
{
    bool WasLoadingSuccessful;
    std::array<int, 8> VertexCoordinates;
};

class PictureLoader
{
    unsigned int _mainTextureId;
    unsigned int _blurryBackgroundTextureId;
    std::shared_ptr<ResolutionScaleCalculator> _resolutionScaleCalculator;
    std::shared_ptr<ImagePositionCalculator> _imagePositionCalculator;
    int _maxDimension;
    unsigned int _vertexArrayId;
    unsigned int _arrayBufferId;
    unsigned int _indexBufferId;
    unsigned int _textureSlot;

public:
    PictureLoader(std::shared_ptr<ResolutionScaleCalculator>, std::shared_ptr<ImagePositionCalculator>);
    PictureLoadResult Load(std::string pathToFile);
    void Render();
};