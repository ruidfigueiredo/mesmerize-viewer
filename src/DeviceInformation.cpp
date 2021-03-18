#include "DeviceInformation.h"

int DeviceInformation::_width = -1;
int DeviceInformation::_height = -1;
int DeviceInformation::_deviceWidth = -1;
int DeviceInformation::_deviceHeight = -1;
int DeviceInformation::_maxTextureSize = -1;

int DeviceInformation::getDeviceWidth()
{
    if (_deviceWidth == -1)
        throw std::logic_error("deviceWidth not initialized, did you forgot to call init?");
    return _deviceWidth;
}

int DeviceInformation::getDeviceHeight()
{
    if (_deviceHeight == -1)
        throw std::logic_error("deviceHeight not initialized, did you forgot to call init?");
    return _deviceHeight;
}

int DeviceInformation::getWidth()
{
    if (_width == -1)
        throw std::logic_error("width not initialized, did you forgot to call init?");
    return _width;
}

int DeviceInformation::getHeight()
{
    if (_height == -1)
        throw std::logic_error("height not initialized, did you forgot to call init?");
    return _height;
}

void DeviceInformation::updateDimensions(GLFWwindow *, int newWidth, int newHeight)
{
    std::cout << "Window dimensions changed to " << newWidth << "x" << newHeight << "\n";
    _width = newWidth;
    _height = newHeight;
    glViewport(0, 0, newWidth, newHeight);
}

int DeviceInformation::getMaxTextureSize()
{
    if (_maxTextureSize == -1)
        throw std::logic_error("maxTextureSize not initialized, did you forgot to call init?");
    return _maxTextureSize;
}

void DeviceInformation::init(GLFWwindow *window, int width, int height)
{
    _width = width;
    _height = height;
    GL_CALL(glfwSetWindowSizeCallback(window, updateDimensions));
    GL_CALL(glGetIntegerv(GL_MAX_TEXTURE_SIZE, &_maxTextureSize));
    const GLFWvidmode *vidmode;
    GL_CALL(vidmode = glfwGetVideoMode(glfwGetPrimaryMonitor()));
    _deviceWidth = vidmode->width;
    _deviceHeight = vidmode->height;
}
