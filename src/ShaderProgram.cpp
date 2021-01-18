#include "ShaderProgram.h"
#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

ShaderProgram::ShaderProgram() : _programId(glCreateProgram()){};

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(_programId);
}

void ShaderProgram::Bind() const
{
    glUseProgram(_programId);
}

void ShaderProgram::Unbind() const
{
    glUseProgram(0);
}

void ShaderProgram::AddFragmentShader(const std::string &pathToFile) const
{
    LoadAndLinkShader(GL_FRAGMENT_SHADER, pathToFile);
}

void ShaderProgram::AddVertexShader(const std::string &pathToFile) const
{
    LoadAndLinkShader(GL_VERTEX_SHADER, pathToFile);
}

int ShaderProgram::GetLocation(const std::string &name)
{
    int location;
    if (_uniformLocations.find(name) != _uniformLocations.end())
    {
        location = _uniformLocations.at(name);
    }
    else
    {
        location = glGetUniformLocation(_programId, name.c_str());
        _uniformLocations[name] = location;
        if (location == -1)
        {
            std::cout << "WARN: Could not find uniform with name " << name << std::endl;
        }
    }

    return location;
}

void ShaderProgram::SetUniform4f(const std::string &name, const float &v1, const float &v2, const float &v3, const float &v4)
{
    int location = GetLocation(name);

    glUniform4f(location, v1, v2, v3, v4);
}

void ShaderProgram::SetUniformi(const std::string &name, const int &value)
{
    int location = GetLocation(name);
    glUniform1i(location, value);
}

void ShaderProgram::SetUniformMat4f(const std::string &name, const glm::mat4 &proj)
{
    int location = GetLocation(name);
    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(proj));
}

void ShaderProgram::LoadAndLinkShader(unsigned int type, const std::string &pathToFile) const
{
    const std::string &shaderSourceCode = GetTextFileContents(pathToFile);
    unsigned int shaderId = CreateShader(_programId, type, shaderSourceCode.c_str());
    glAttachShader(_programId, shaderId);
    glLinkProgram(_programId);
    glValidateProgram(_programId);
    glDeleteShader(shaderId);
}

const std::string ShaderProgram::GetTextFileContents(const std::string &filePath) const
{
    std::ifstream fileStream(filePath);
    if (!fileStream.is_open())
        throw "Could not open file:" + filePath;

    std::string line;
    std::stringstream ss;
    while (getline(fileStream, line))
    {
        ss << line << std::endl;
    }
    return ss.str();
}

const unsigned int ShaderProgram::CreateShader(unsigned int programId, unsigned int type, const char *source) const
{
    const unsigned int shaderId = glCreateShader(type);
    glShaderSource(shaderId, 1, &source, nullptr);
    glCompileShader(shaderId);

    int compileStatusFlag;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatusFlag);
    if (compileStatusFlag == GL_FALSE)
    {
        int logLength;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength);
        char *errorMessage = new char[logLength];
        glGetShaderInfoLog(shaderId, logLength, &logLength, errorMessage);
        std::cout << "Error compiling shader: " << std::endl;
        std::cout << "type: " << (type == GL_VERTEX_SHADER ? "Vertex shader" : "Fragment shader") << std::endl;
        std::cout << errorMessage;
        delete[] errorMessage;
    }

    return shaderId;
}
