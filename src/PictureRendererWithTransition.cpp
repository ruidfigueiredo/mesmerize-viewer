#include "PictureRendererWithTransition.h"
#include "ResolutionScaleCalculator.h"
#include "ImagePositionCalculator.h"
#include "TimingFunctions/EaseInOut.h"
#include <iostream>

const int PictureRendererWithTransition::_panAnimationDuration = 40000;
const int PictureRendererWithTransition::_zoomAnimationDuration = 60000;
const int PictureRendererWithTransition::_opacityAnimationDuration = 1000;
const float PictureRendererWithTransition::_percentageToMove = 0.05f;
const float PictureRendererWithTransition::_percentageToZoom = 0.05f;


PictureRendererWithTransition::PictureRendererWithTransition()
: _currentPictureIndex{-1},
  _state(PictureRendererWithTransitionState::EMPTY),
  _panTimingFunction(std::make_shared<EaseInOut>(_panAnimationDuration)),
  _zoomTimingFunction(std::make_shared<EaseInOut>(_zoomAnimationDuration)),
  _opacityTimingFunction(std::make_shared<EaseInOut>(_opacityAnimationDuration, false, [this](){
      _state = PictureRendererWithTransitionState::TWO;
      Swap();
  })),
  _pictures {
    Picture(std::make_shared<ResolutionScaleCalculator>(), std::make_shared<ImagePositionCalculator>()),
    Picture(std::make_shared<ResolutionScaleCalculator>(), std::make_shared<ImagePositionCalculator>())
  }
 {
 }

void PictureRendererWithTransition::Load(std::string path) {
    int loadIndex = GetBackPictureIndex();
    std::cout << "Loading picture into slot " << loadIndex << std::endl;
    _pictures[loadIndex].Load(path, loadIndex, PictureScaleMode::COVER, PictureEffects::NONE, [loadIndex, this](){
        if (_state == PictureRendererWithTransitionState::EMPTY) {
            _state = PictureRendererWithTransitionState::ONE;
        }else {
            _state = PictureRendererWithTransitionState::TRANSITIONING;
            _opacityTimingFunction->Reset();
        }
    });
}

void PictureRendererWithTransition::Render() {
    if (_state != PictureRendererWithTransitionState::EMPTY) {
        float numberOfWidthPixelsToMove = DeviceInformation::getWidth() * _percentageToMove;
        float numberOfHeightPixelsToMove = 0.0f;

        float scaleFactor = _percentageToZoom * _zoomTimingFunction->GetValue() + _percentageToMove;
        float scaleXTranslationAdjustment = scaleFactor * DeviceInformation::getWidth() / 2 + _percentageToMove;
        float scaleYTranslationAdjustment = scaleFactor * DeviceInformation::getHeight() / 2 + _percentageToMove;

        glm::mat4 projection = glm::ortho(0.0f, 1.0f * DeviceInformation::getWidth(), 0.0f, 1.0f * DeviceInformation::getHeight(), -1.0f, 1.0f);
        glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f + scaleFactor, 1.0f + scaleFactor, 1.0f});
        glm::mat4 view = glm::translate(model, glm::vec3{(numberOfWidthPixelsToMove * _panTimingFunction->GetValue() - numberOfWidthPixelsToMove / 2) - scaleXTranslationAdjustment, (numberOfHeightPixelsToMove*_panTimingFunction->GetValue() - numberOfHeightPixelsToMove /2) -scaleYTranslationAdjustment, 0.0f});

        if (_state == PictureRendererWithTransitionState::ONE || _state == PictureRendererWithTransitionState::TWO)
            _pictures[GetFrontPictureIndex()].Render(projection*view*model);
        else {
            _pictures[GetBackPictureIndex()].Render(projection*view*model);
            float opacity = _opacityTimingFunction->GetValue();
            _pictures[GetFrontPictureIndex()].Render(projection * view * model, 1- opacity);
        }
    }
}

int PictureRendererWithTransition::GetFrontPictureIndex() {
    return _currentPictureIndex == - 1 ? 0 : _currentPictureIndex;
}

int PictureRendererWithTransition::GetBackPictureIndex() {
    return _currentPictureIndex == - 1 ? 0 : (_currentPictureIndex + 1) % 2;
}

void PictureRendererWithTransition::Swap() {
    _currentPictureIndex = (_currentPictureIndex + 1) % 2;
}