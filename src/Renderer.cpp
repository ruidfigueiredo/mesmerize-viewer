#include "Renderer.h"
#include <GL/glew.h>

void Renderer::Draw(const VertexArray &vertexArray, const ShaderProgram &shaderProgram)
{
    vertexArray.Bind();
    shaderProgram.Bind();
    glDrawElements(GL_TRIANGLES, vertexArray.GetElementsCount(), vertexArray.GetElementsType(), nullptr);
}

void Renderer::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
}