#include "PictureRendererWithTransition.h"
#include "ResolutionScaleCalculator.h"
#include "ImagePositionCalculator.h"
#include <memory>

PictureRendererWithTransition::PictureRendererWithTransition()
: _currentPictureIndex{-1},
  _pictures {
    Picture(std::make_shared<ResolutionScaleCalculator>(), std::make_shared<ImagePositionCalculator>()),
    Picture(std::make_shared<ResolutionScaleCalculator>(), std::make_shared<ImagePositionCalculator>())
  }
 {
 }

void PictureRendererWithTransition::Load(std::string path) {
    _pictures[_currentPictureIndex + 1].Load(path, _currentPictureIndex + 1, PictureScaleMode::COVER, PictureEffects::NONE, [this](){
        _currentPictureIndex = _currentPictureIndex + 1 % 2;
    });
}

void PictureRendererWithTransition::Render() {
    if (_currentPictureIndex != -1) {
        _pictures[_currentPictureIndex].Render();
    }
}
