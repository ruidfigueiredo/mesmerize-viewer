#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "CheckGlErrors.h"
#include <iostream>
#include "CallbackRegistry.h"
#include <functional>

class DeviceInformation{
    static int _deviceWidth;
    static int _deviceHeight;
    static int _width;
    static int _height;
    static int _maxTextureSize;
    static CallbackRegistry<std::function<void(int, int)>> _sizeChangedCallbackRegistry;

    public:
    static int getDeviceWidth();
    static int getDeviceHeight();
    static int getWidth();
    static int getHeight();

    static void updateDimensions(GLFWwindow*, int newWidth, int newHeight);

    static int getMaxTextureSize();

    static void init(GLFWwindow* window, int width, int height);

    static void registerSizeChangedCallback(std::function<void(int, int)>&& callback, void *owner = nullptr);
    static void unRegisterSizeChangedCallback(void *owner);
};
