#include "VertexBuffer.h"
#include <GL/glew.h>
#include "CheckGlErrors.h"

VertexBuffer::VertexBuffer(const void *data, unsigned int size)
{
    GL_CALL(glGenBuffers(1, &_rendererId));
    this->Bind();
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::~VertexBuffer()
{
    GL_CALL(glDeleteBuffers(1, &_rendererId));
}

void VertexBuffer::Bind() const
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, _rendererId));
}

void VertexBuffer::Unbind() const
{
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
}
