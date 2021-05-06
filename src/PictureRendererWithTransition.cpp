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
    int loadIndex = (_currentPictureIndex + 1) % 1;
    _pictures[loadIndex].Load(path, loadIndex, PictureScaleMode::COVER, PictureEffects::NONE, [loadIndex, this](){
        _currentPictureIndex = loadIndex;
    });
}

void PictureRendererWithTransition::Render() {
    if (_currentPictureIndex != -1) {
        _pictures[_currentPictureIndex].Render();
    }
}
