#include "PictureRendererWithTransition.h"
#include "ResolutionScaleCalculator.h"
#include "ImagePositionCalculator.h"
#include "TimingFunctions/EaseInOut.h"

const int PictureRendererWithTransition::_animationDuration = 25000;

PictureRendererWithTransition::PictureRendererWithTransition()
: _currentPictureIndex{-1},
  _timingFunction{std::make_shared<EaseInOut>(_animationDuration)},
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
void PictureRendererWithTransition::Render() {
    if (_currentPictureIndex != -1) {
        if (percentageToMove < 0.5f)
            percentageToMove+=0.00005f;
        float numberOfWidthPixelsToMove = DeviceInformation::getWidth() * percentageToMove;
        float numberOfHeightPixelsToMove = DeviceInformation::getHeight() * percentageToMove;

        glm::mat4 projection = glm::ortho(0.0f, 1.0f * DeviceInformation::getWidth(), 0.0f, 1.0f * DeviceInformation::getHeight(), -1.0f, 1.0f);
        glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f + percentageToMove, 1.0f + percentageToMove, 1.0f});
        glm::mat4 view = glm::translate(model, glm::vec3{numberOfWidthPixelsToMove * _timingFunction->GetValue() - numberOfWidthPixelsToMove / 2 -numberOfWidthPixelsToMove/2, -numberOfHeightPixelsToMove/2, 0.0f});


        _pictures[_currentPictureIndex].Render(projection*view*model);
    }
}
