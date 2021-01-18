#pragma once
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

class VertexArray
{
private:
    unsigned int _rendererId;
    bool _hasIndexBuffer;
    unsigned int _numberOfElementsInIndexBuffer;

public:
    VertexArray();
    ~VertexArray();
    void AddBuffer(const VertexBuffer &buffer, const VertexBufferLayout &layout) const;
    void AddBuffer(const IndexBuffer &buffer);
    void Bind() const;
    void Unbind() const;
    unsigned long GetElementsCount() const;
    unsigned int GetElementsType() const;
};