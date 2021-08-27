#pragma once

#include <string>
#include <iostream>

class MesmerizeOptions {

public:
    MesmerizeOptions();

    static MesmerizeOptions FromArgs(int argc, const char** argv);
    static void PrintOptions();

    bool IsFullScreen;
    bool IsResolutionSetToMax;
    int ResolutionX;
    int ResolutionY;
    int DisplayPictureDuration;
    int OpacityAnimationDuration;
    int ZoomAnimationDuration;
    int PanAnimationDuration;
    float ZoomPercentage;
    float PanPercentage;
    std::string PathToPicturesFolder;
};

std::ostream& operator<<(std::ostream& out, const MesmerizeOptions &mesmerizeOptions);
