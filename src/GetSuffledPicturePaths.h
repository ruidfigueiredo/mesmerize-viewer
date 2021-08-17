#pragma once
#include <algorithm>
#include "GetFilesPathsRecursively.h"
#include <ctype.h>
#include "shuffle.h"

namespace GetShuffledPicturePaths {

    std::string toLowerCase(const std::string& str){
        std::string lowerCaseString = str;
        std::transform(str.begin(), str.end(), lowerCaseString.begin(), [](unsigned char c) {
            return tolower(c);
        });
        return lowerCaseString;
    }

    bool isImageFile(const std::string& str)
    {
        std::string imagePath = toLowerCase(str);

        static const std::vector<std::string> supportedImageExtensions = {".jpg", ".png"};
        for(const auto& supportedExtension : supportedImageExtensions) {
            if (toLowerCase(str).find(supportedExtension) == str.size() - supportedExtension.size()){
                return true;
            }
        }
        return false;
    }

    std::vector<std::string> GetShuffledPicturePaths(const std::string& picturesPath) {
        auto allFiles = GetFilePathsRecursively(picturesPath);
        auto picturePaths = std::vector<std::string>();
        std::copy_if(allFiles.begin(), allFiles.end(), std::back_inserter(picturePaths), [](const std::string& path) {
            return isImageFile(path);
        });

        shuffle(picturePaths);
        return picturePaths;
    }
}

