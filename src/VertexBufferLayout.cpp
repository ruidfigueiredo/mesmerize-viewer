#include "VertexBufferLayout.h"

template <>
void VertexBufferLayout::Push<float>(int count)
{
    _attributes.push_back({GL_FLOAT, count});
    _stride += BufferAttributes::GetSizeOfType(GL_FLOAT) * count;
}

unsigned long VertexBufferLayout::GetStride() const
{
    return _stride;
}