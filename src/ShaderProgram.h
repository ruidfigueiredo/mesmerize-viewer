#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

class ShaderProgram
{
private:
    int _programId;
    std::unordered_map<std::string, int> _uniformLocations;

public:
    ShaderProgram();
    ~ShaderProgram();
    void AddVertexShader(const std::string &pathToShader) const;
    void AddFragmentShader(const std::string &pathToShader) const;
    void SetUniformi(const std::string &name, const int &v1);
    void SetUniformf(const std::string &name, const float &v1);
    void SetUniform4f(const std::string &name, const float &v1, const float &v2, const float &v3, const float &v4);
    void SetUniformMat4f(const std::string &name, const glm::mat4 &proj);
    void Bind() const;
    void Unbind() const;

private:
    const std::string GetTextFileContents(const std::string &filePath) const;
    const unsigned int CreateShader(unsigned int programId, unsigned int type, const char *source) const;
    void LoadAndLinkShader(unsigned int type, const std::string &filePath) const;
    int GetLocation(const std::string &name);
};