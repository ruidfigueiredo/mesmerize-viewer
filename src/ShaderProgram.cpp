#include "ShaderProgram.h"
#include <GL/glew.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "CheckGlErrors.h"
#include <sys/stat.h>

ShaderProgram::ShaderProgram() : _programId(-2){};

ShaderProgram::~ShaderProgram()
{
    if (_programId != -2)
        GL_CALL(glDeleteProgram(_programId));
}

void ShaderProgram::Init()
{
    _programId = glCreateProgram();
}

void ShaderProgram::Bind() const
{
    GL_CALL(glUseProgram(_programId));
}

void ShaderProgram::Unbind() const
{
    GL_CALL(glUseProgram(0));
}

void ShaderProgram::AddFragmentShader(const std::string &pathToFile) const
{
    const std::string &shaderSourceCode = GetTextFileContents(pathToFile);
    LoadAndLinkShader(GL_FRAGMENT_SHADER, shaderSourceCode.c_str());
}

void ShaderProgram::AddVertexShader(const std::string &pathToFile) const
{
    const std::string &shaderSourceCode = GetTextFileContents(pathToFile);
    LoadAndLinkShader(GL_VERTEX_SHADER, shaderSourceCode.c_str());
}

void ShaderProgram::AddVertexShaderCode(const char* sourceCode) const
{
    LoadAndLinkShader(GL_VERTEX_SHADER, sourceCode);
}

void ShaderProgram::AddFragmentShaderCode(const char* sourceCode) const
{
    LoadAndLinkShader(GL_FRAGMENT_SHADER, sourceCode);
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

    GL_CALL(glUniform4f(location, v1, v2, v3, v4));
}

void ShaderProgram::SetUniformi(const std::string &name, const int &value)
{
    int location = GetLocation(name);
    GL_CALL(glUniform1i(location, value));
}

void ShaderProgram::SetUniformf(const std::string &name, const float &value)
{
    int location = GetLocation(name);
    GL_CALL(glUniform1f(location, value));
}

void ShaderProgram::SetUniformMat4f(const std::string &name, const glm::mat4 &proj)
{
    int location = GetLocation(name);
    GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(proj)));
}

void ShaderProgram::LoadAndLinkShader(unsigned int type, const char* shaderCode) const
{
    unsigned int shaderId = CreateShader(type, shaderCode);
    GL_CALL(glAttachShader(_programId, shaderId));
    GL_CALL(glLinkProgram(_programId));
    GL_CALL(glValidateProgram(_programId));
    GL_CALL(glDeleteShader(shaderId));
}

const std::string ShaderProgram::GetTextFileContents(const std::string &filePath) const
{

    struct stat statResult;
    if (stat(filePath.c_str(), &statResult) == -1) {
        throw std::runtime_error(filePath + " not found (with shader code)");
    }
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

const unsigned int ShaderProgram::CreateShader(unsigned int type, const char *source) const
{
    const unsigned int shaderId = glCreateShader(type);
    GL_CALL(glShaderSource(shaderId, 1, &source, nullptr));
    GL_CALL(glCompileShader(shaderId));

    int compileStatusFlag;
    GL_CALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &compileStatusFlag));
    if (compileStatusFlag == GL_FALSE)
    {
        int logLength;
        GL_CALL(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &logLength));
        char errorMessage[logLength];
        GL_CALL(glGetShaderInfoLog(shaderId, logLength, &logLength, errorMessage));
        std::cout << "Error compiling shader: " << std::endl;
        std::cout << "type: " << (type == GL_VERTEX_SHADER ? "Vertex shader" : "Fragment shader") << std::endl;
        std::cout << errorMessage;
        throw std::string(errorMessage);
    }

    return shaderId;
}
