#include "PictureRendererWithTransition.h"
#include "ResolutionScaleCalculator.h"
#include "ImagePositionCalculator.h"
#include "TimingFunctions/EaseInOut.h"

const int PictureRendererWithTransition::_panAnimationDuration = 25000;
const int PictureRendererWithTransition::_zoomAnimationDuration = 50000;

PictureRendererWithTransition::PictureRendererWithTransition()
: _currentPictureIndex{-1},
  _panTimingFunction(std::make_shared<EaseInOut>(_panAnimationDuration)),
  _zoomTimingFunction(std::make_shared<EaseInOut>(_zoomAnimationDuration)),
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
float percentageToMove = 0.1f;
float percentageToZoom = 0.3f;
void PictureRendererWithTransition::Render() {
    if (_currentPictureIndex != -1) {
        float numberOfWidthPixelsToMove = DeviceInformation::getWidth() * percentageToMove;
        float numberOfHeightPixelsToMove = DeviceInformation::getHeight() * percentageToMove;

        float scaleFactor = percentageToMove * _zoomTimingFunction->GetValue() + percentageToMove;
        float scaleXTranslationAdjustment = scaleFactor * DeviceInformation::getWidth() / 2 + percentageToMove;
        float scaleYTranslationAdjustment = scaleFactor * DeviceInformation::getHeight() / 2 + percentageToMove;

        glm::mat4 projection = glm::ortho(0.0f, 1.0f * DeviceInformation::getWidth(), 0.0f, 1.0f * DeviceInformation::getHeight(), -1.0f, 1.0f);
        glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f + scaleFactor, 1.0f + scaleFactor, 1.0f});
        glm::mat4 view = glm::translate(model, glm::vec3{(numberOfWidthPixelsToMove * _panTimingFunction->GetValue() - numberOfWidthPixelsToMove / 2) - scaleXTranslationAdjustment, (numberOfHeightPixelsToMove*_panTimingFunction->GetValue() - numberOfHeightPixelsToMove /2) -scaleYTranslationAdjustment, 0.0f});


        _pictures[_currentPictureIndex].Render(projection*view*model);
    }
}
