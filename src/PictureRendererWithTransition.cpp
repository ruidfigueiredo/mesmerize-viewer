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
: _currentPictureIndex{-1},
  _state(PictureRendererWithTransitionState::EMPTY),
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
     //_pictures[GetBackPictureIndex()].Load("/home/rdfi/Pictures/Danny.jpg", GetBackPictureIndex());
     //_pictures[GetFrontPictureIndex()].Load("/home/rdfi/Pictures/Alex.jpg", GetFrontPictureIndex());
 }

void PictureRendererWithTransition::Load(std::string path) {
    //int loadIndex = _currentPictureIndex == -1 ? GetFrontPictureIndex() : GetBackPictureIndex();
    int loadIndex = GetFrontPictureIndex();
    std::cout << "Loading picture into slot " << loadIndex << std::endl;
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
    //if (_state != PictureRendererWithTransitionState::EMPTY) {
        float numberOfWidthPixelsToMove = DeviceInformation::getWidth() * _percentageToMove;
        float numberOfHeightPixelsToMove = 0.0f;

        float scaleFactor = _percentageToZoom * _zoomTimingFunction->GetValue() + _percentageToMove;
        float scaleXTranslationAdjustment = scaleFactor * DeviceInformation::getWidth() / 2 + _percentageToMove;
        float scaleYTranslationAdjustment = scaleFactor * DeviceInformation::getHeight() / 2 + _percentageToMove;

        glm::mat4 projection = glm::ortho(0.0f, 1.0f * DeviceInformation::getWidth(), 0.0f, 1.0f * DeviceInformation::getHeight(), -1.0f, 1.0f);
        glm::mat4 model = glm::scale(glm::mat4{1.0f}, glm::vec3{1.0f + scaleFactor, 1.0f + scaleFactor, 1.0f});
        glm::mat4 view = glm::translate(model, glm::vec3{(numberOfWidthPixelsToMove * _panTimingFunction->GetValue() - numberOfWidthPixelsToMove / 2) - scaleXTranslationAdjustment, (numberOfHeightPixelsToMove*_panTimingFunction->GetValue() - numberOfHeightPixelsToMove /2) -scaleYTranslationAdjustment, 0.0f});

        glm::mat4 projectionMatrix = projection*view*model;

        float opacity = _state == PictureRendererWithTransitionState::TRANSITIONING ? _opacityTimingFunction->GetValue() : 1;
        _pictures[GetBackPictureIndex()].Render(projectionMatrix);
        _pictures[GetFrontPictureIndex()].Render(projectionMatrix, 1-opacity);


/*        if (_state == PictureRendererWithTransitionState::ONE || _state == PictureRendererWithTransitionState::TWO) {
            std::cout << "Rendering slot: " << GetFrontPictureIndex() << "\n";
            _pictures[GetFrontPictureIndex()].Render(projectionMatrix);
        } else {
            float opacity = _opacityTimingFunction->GetValue();
            _pictures[GetBackPictureIndex()].Render(projectionMatrix);
            _pictures[GetFrontPictureIndex()].Render(projectionMatrix, 1-opacity);
            std::cout << "Rendering dual slots: " << GetFrontPictureIndex() << ", " << GetBackPictureIndex() << "\n";
        }*/
    //}
}

int PictureRendererWithTransition::GetFrontPictureIndex() {
    return (_currentPictureIndex + 1) % 2;
}

int PictureRendererWithTransition::GetBackPictureIndex() {
    return (_currentPictureIndex + 2) % 2;
}

void PictureRendererWithTransition::Swap() {
    _currentPictureIndex = (_currentPictureIndex + 1) % 2;
}