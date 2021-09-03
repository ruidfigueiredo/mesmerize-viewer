#include <iostream>
#include "MesmerizeOptions.h"

MesmerizeOptions::MesmerizeOptions() :
    IsFullScreen(false),
    IsResolutionSetToMax(true),
    DisplayPictureDuration(15000),
    OpacityAnimationDuration(4000),
    ZoomAnimationDuration(20000),
    PanAnimationDuration(10000),
    ZoomPercentage(.05f),
    PanPercentage(.05f),
    ResolutionX(-1),
    ResolutionY(-1) { }


void MesmerizeOptions::PrintOptions() {
    std::cout << "usage: [--window|--full-screen] [--resolution=widthxheight|--resolution=max] [--display-picture-duration=duration_in_seconds]\n"\
                 "\t[--opacity-animation-duration=duration_in_seconds] [--zoom-animation-duration=duration_in_seconds]\n"\
                 "\t[--pan-animation-duration=duration_in_seconds] [--zoom-percentage=number_1_to_100]\n"\
                 "\t[--pan-percentage=number_1_to_100] path_to_folder_with_images_jpg_and_png\n"\
                 "\n\nDefaults:\n"\
                 "\t--window\n"\
                 "\t--resolution=max (1920x1080 is what works best on a pi 4)\n"\
                 "\t--display-picture-duration=15\n"\
                 "\t--opacity-animation-duration=4\n"\
                 "\t--zoom-animation-duration=20\n"\
                 "\t--pan-animation-duration=10\n"\
                 "\t--zoom-percentage=5\n"\
                 "\t--pan-percentage=5\n";
}

MesmerizeOptions MesmerizeOptions::FromArgs(int argc, const char** argv) {
    MesmerizeOptions options;
    std::string path = "";
    for(int i = 1; i < argc; i++) {
        std::string arg(argv[i]);
        if (arg.find("--") == std::string::npos) {
            if (path.compare("") != 0)
                throw std::runtime_error("Invalid arg " + arg);

            path = arg; //we assume that the non-option (no --) is the path
        }else{
            std::string optionName;
            std::string optionValue;

            if (arg.find("=") == std::string::npos) {
                optionName = arg.substr(2);
                optionValue = "";
            } else {
                optionName = arg.substr(2, arg.find("=") - 2);
                optionValue = arg.substr(arg.find("=") + 1);
            }
            if (!optionName.compare("full-screen")){
                options.IsFullScreen = true;
            }else if (!optionName.compare("window")){
                options.IsFullScreen = false;
            }else if (!optionName.compare("resolution")) {
                if (!optionValue.find("max")) {
                    options.IsResolutionSetToMax = true;
                    options.ResolutionX = -1;
                    options.ResolutionY = -1;
                } else {
                    if (optionValue.find("x") == std::string::npos)
                        throw std::runtime_error(
                                "resolution option value is using an incorrect format, correct format is widthxheight");
                    std::string resXstr = optionValue.substr(0, optionValue.find("x"));
                    std::string resYstr = optionValue.substr(optionValue.find("x") + 1);
                    options.ResolutionX = std::stoi(resXstr);
                    options.ResolutionY = std::stoi(resYstr);
                }
            }else if (!optionName.compare("display-picture-duration")) {
                int durationInSeconds = std::stoi(optionValue);
                options.DisplayPictureDuration = durationInSeconds * 1000;
            }else if (!optionName.compare("opacity-animation-duration")) {
                int durationInSeconds = std::stoi(optionValue);
                options.OpacityAnimationDuration = durationInSeconds * 1000;
            }else if (!optionName.compare("zoom-animation-duration")) {
                int durationInSeconds = std::stoi(optionValue);
                options.ZoomAnimationDuration = durationInSeconds * 1000;
            }else if (!optionName.compare("pan-animation-duration")) {
                int durationInSeconds = std::stoi(optionValue);
                options.PanAnimationDuration = durationInSeconds * 1000;
            }else if (!optionName.compare("zoom-percentage")) {
                int zoomPercentage = std::stoi(optionValue);
                options.ZoomPercentage = zoomPercentage / 100.0f;
            }else if (!optionName.compare("pan-percentage")) {
                int panPercentage = std::stoi(optionValue);
                options.PanPercentage = panPercentage / 100.0f;
            }else {
                throw std::runtime_error("Unknown option: " + optionName);
            }
        }
    }



    if (path.empty()){
        throw std::runtime_error("Path to root picture folder missing");
    }else {
        options.PathToPicturesFolder = path;
    }
    return options;
}
std::ostream& operator<<(std::ostream& out, const MesmerizeOptions& mesmerizeOptions)
{
    out << "IsFullScreen: " << std::boolalpha << mesmerizeOptions.IsFullScreen << std::noboolalpha << "\n";
    out << "Resolution: " << mesmerizeOptions.ResolutionX << "x" << mesmerizeOptions.ResolutionY << "\n";
    out << "DisplayPictureDuration: " << mesmerizeOptions.DisplayPictureDuration << "ms\n";
    out << "OpacityAnimationDuration: " << mesmerizeOptions.OpacityAnimationDuration << "ms\n";
    out << "ZoomAnimationDuration: " << mesmerizeOptions.ZoomAnimationDuration << "ms\n";
    out << "PanAnimationDuration: " << mesmerizeOptions.PanAnimationDuration << "ms\n";
    out << "ZoomPercentage: " << mesmerizeOptions.ZoomPercentage << "\n";
    out << "PanPercentage: " << mesmerizeOptions.PanPercentage << "\n";
    out << "Path to root picture folder: " << mesmerizeOptions.PathToPicturesFolder << "\n";
    return out;
}
