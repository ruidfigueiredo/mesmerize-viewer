#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "CheckGlErrors.h"
#include <iostream>

class DeviceInformation{
    static int _deviceWidth;
    static int _deviceHeight;
    static int _width;
    static int _height;
    static int _maxTextureSize;
    public:
    static int getDeviceWidth();
    static int getDeviceHeight();
    static int getWidth();
    static int getHeight();

    static void updateDimentions(GLFWwindow*, int newWidth, int newHeight);

    static int getMaxTextureSize();

    static void init(GLFWwindow* window, int width, int height);
};
