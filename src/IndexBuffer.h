#pragma once

class IndexBuffer
{
private:
    unsigned int _rendererId;
    unsigned int _count;

public:
    IndexBuffer(const unsigned int *data, unsigned int count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;

    unsigned int GetNumberOfElements() const;
};