#include "PictureRendererWithTransition.h"
#include "ResolutionScaleCalculator.h"
#include "ImagePositionCalculator.h"
#include "TimingFunctions/EaseInOut.h"
#include <iostream>

const int PictureRendererWithTransition::_panAnimationDuration = 40000;
const int PictureRendererWithTransition::_zoomAnimationDuration = 60000;
const int PictureRendererWithTransition::_opacityAnimationDuration = 4000;
const float PictureRendererWithTransition::_percentageToMove = 0.05f;
const float PictureRendererWithTransition::_percentageToZoom = 0.05f;


PictureRendererWithTransition::PictureRendererWithTransition()
: _currentLoadedPictureIndex{0},
  _state(PictureRendererWithTransitionState::ONE),
  _panTimingFunction(std::make_shared<EaseInOut>(_panAnimationDuration)),
  _zoomTimingFunction(std::make_shared<EaseInOut>(_zoomAnimationDuration)),
  _opacityTimingFunction(std::make_shared<EaseInOut>(_opacityAnimationDuration, false, [this](){
      _state = PictureRendererWithTransitionState::TWO;
      std::cout << "Opacity ended\n";
      Swap();
  })),
  _pictures {
    Picture(std::make_shared<ResolutionScaleCalculator>(), std::make_shared<ImagePositionCalculator>()),
    Picture(std::make_shared<ResolutionScaleCalculator>(), std::make_shared<ImagePositionCalculator>())
  }
 {
 }

void PictureRendererWithTransition::Load(std::string path) {
    int loadIndex = _currentLoadedPictureIndex;
    std::cout << "Loading picture into slot " << loadIndex << std::endl;
    //std::cout << "Visible is: " << GetVisiblePictureIndex() << ", Occluded is: " << GetOccludedPictureIndex() << std::endl;
    _pictures[loadIndex].Load(path, loadIndex, PictureScaleMode::COVER, PictureEffects::NONE, [this](){
        if (_state == PictureRendererWithTransitionState::EMPTY) {
            _state = PictureRendererWithTransitionState::ONE;
        }else {
            _state = PictureRendererWithTransitionState::TRANSITIONING;
            _opacityTimingFunction->Reset();
        }
    });
}

void PictureRendererWithTransition::Render() {
    float numberOfWidthPixelsToMove = DeviceInformation::getWidth() * _percentageToMove;
    float numberOfHeightPixelsToMove = 0.0f;

    float scaleFactor = _percentageToZoom * _zoomTimingFunction->GetValue() + _percentageToMove;
    float scaleXTranslationAdjustment = scaleFactor * DeviceInformation::getWidth() / 2 + _percentageToMove;
    float scaleYTranslationAdjustment = scaleFactor * DeviceInformation::getHeight() / 2 + _percentageToMove;

    glm::mat4 projection = glm::ortho(0.0f, 1.0f * DeviceInformation::getWidth(), 0.0f, 1.0f * DeviceInformation::getHeight(), -1.0f, 1.0f);
    glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f + scaleFactor, 1.0f + scaleFactor, 1.0f});
    glm::mat4 view = glm::translate(model, glm::vec3{(numberOfWidthPixelsToMove * _panTimingFunction->GetValue() - numberOfWidthPixelsToMove / 2) - scaleXTranslationAdjustment, (numberOfHeightPixelsToMove*_panTimingFunction->GetValue() - numberOfHeightPixelsToMove /2) -scaleYTranslationAdjustment, 0.0f});

    glm::mat4 projectionMatrix = projection*view*model;

    //float opacity = _state == PictureRendererWithTransitionState::TRANSITIONING ? _opacityTimingFunction->GetValue() : 0;
    float opacityFront = 0;
    float opacityBack = 0;
    if (_state == PictureRendererWithTransitionState::TRANSITIONING) {
        if (_currentLoadedPictureIndex == 0) { //going from 0 to 1
            opacityFront = _opacityTimingFunction->GetValue();
            opacityBack = 1 - opacityFront;
        }else { //going from 1 to 0
            opacityBack = _opacityTimingFunction->GetValue();
            opacityFront = 1 - opacityBack;
        }
    }else {
        if (_currentLoadedPictureIndex == 0) {
            opacityFront = 0;
            opacityBack = 1;
        }else {
            opacityFront = 1;
            opacityBack = 0;
        }
    }
    _pictures[1].Render(projectionMatrix, opacityBack);
    _pictures[0].Render(projectionMatrix, opacityFront);
}

int PictureRendererWithTransition::GetVisiblePictureIndex() const {
    return (_currentLoadedPictureIndex + 1) % 2;
}

int PictureRendererWithTransition::GetOccludedPictureIndex() const {
    return (_currentLoadedPictureIndex + 2) % 2;
}

void PictureRendererWithTransition::Swap() {
    _currentLoadedPictureIndex = (_currentLoadedPictureIndex + 1) % 2;
    std::cout << "Swap happened, visible is now: " << GetVisiblePictureIndex() << ", occluded is now: " << GetOccludedPictureIndex() << std::endl;
}