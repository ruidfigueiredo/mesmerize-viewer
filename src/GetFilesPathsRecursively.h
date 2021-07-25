#pragma once
#include <vector>
#include <string>
#include <dirent.h>


void _getFilePathsRecursively(const std::string path, std::vector<std::string> &accumulator) {
    DIR* directory = opendir(path.c_str());
    dirent *directoryEntry = nullptr;

    while((directoryEntry = readdir(directory))) {
        if (directoryEntry->d_type == DT_REG) {
            accumulator.push_back(path + "/" + directoryEntry->d_name);
        }else if (directoryEntry->d_type == DT_DIR && strcmp(directoryEntry->d_name, ".") != 0 && strcmp(directoryEntry->d_name, "..")) {
            _getFilePathsRecursively(path + "/" + directoryEntry->d_name, accumulator);
        }
    }
    closedir(directory);
}


std::vector<std::string> GetFilePathsRecursively(const std::string path) {
    std::vector<std::string> result;

    if (path.empty()){
        return result;
    }

    std::string nonTrailingSlashPath = path;
    if (path[path.size()-1] == '/') {
        nonTrailingSlashPath = path.substr(0, path.size() - 1);
    }

    _getFilePathsRecursively(nonTrailingSlashPath, result);

    return result;
}
