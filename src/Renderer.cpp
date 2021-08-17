#include "Renderer.h"
#include <GL/glew.h>
#include "CheckGlErrors.h"

void Renderer::Draw(const VertexArray &vertexArray, const ShaderProgram &shaderProgram)
{
    vertexArray.Bind();
    shaderProgram.Bind();
    GL_CALL(glDrawElements(GL_TRIANGLES, vertexArray.GetElementsCount(), vertexArray.GetElementsType(), nullptr));
}

void Renderer::Clear()
{
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}