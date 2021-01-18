#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"

VertexArray::VertexArray() : _rendererId(0), _hasIndexBuffer(false), _numberOfElementsInIndexBuffer(-1)
{
    glGenVertexArrays(1, &_rendererId);
    this->Bind();
}

VertexArray::~VertexArray()
{
    glDeleteVertexArrays(1, &_rendererId);
}

void VertexArray::AddBuffer(const VertexBuffer &vb, const VertexBufferLayout &layout) const
{
    vb.Bind();
    const auto &attributes = layout.GetAttributes();
    unsigned long offset = 0;
    for (size_t i = 0; i < attributes.size(); i++)
    {
        const auto &attribute = attributes[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i, attribute.count, attribute.type, GL_FALSE, layout.GetStride(), (const void *)offset);
        offset += attribute.count * BufferAttributes::GetSizeOfType(attribute.type);
    }
}

void VertexArray::AddBuffer(const IndexBuffer &ib)
{
    ib.Bind();
    _hasIndexBuffer = true;
    _numberOfElementsInIndexBuffer = ib.GetNumberOfElements();
}

void VertexArray::Bind() const
{
    glBindVertexArray(_rendererId);
}

void VertexArray::Unbind() const
{
    glBindVertexArray(0);
}

unsigned long VertexArray::GetElementsCount() const
{
    if (!_hasIndexBuffer)
    {
        std::__throw_logic_error("Index buffer must be set");
    }
    return _numberOfElementsInIndexBuffer;
}
unsigned int VertexArray::GetElementsType() const
{
    return GL_UNSIGNED_INT; //TODO: get this from index buffer
}