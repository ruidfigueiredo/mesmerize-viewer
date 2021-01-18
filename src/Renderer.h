#pragma once
#include "VertexArray.h"
#include "ShaderProgram.h"

class Renderer
{
public:
    void Clear();
    void Draw(const VertexArray &vertexArray, const ShaderProgram &shaderProgram);
};