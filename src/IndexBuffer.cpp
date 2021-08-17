#include "IndexBuffer.h"
#include <GL/glew.h>
#include "CheckGlErrors.h"

IndexBuffer::IndexBuffer(const unsigned int *data, unsigned int count) : _count(count)
{
    GL_CALL(glGenBuffers(1, &_rendererId));
    Bind();
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::~IndexBuffer()
{
    GL_CALL(glDeleteBuffers(1, &_rendererId));
}

void IndexBuffer::Bind() const
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _rendererId));
}

void IndexBuffer::Unbind() const
{
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

unsigned int IndexBuffer::GetNumberOfElements() const
{
    return _count;
}