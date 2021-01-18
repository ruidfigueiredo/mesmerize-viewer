#pragma once
#include <vector>
#include <GL/glew.h>

struct BufferAttributes
{
    unsigned int type;
    int count;

    static unsigned long GetSizeOfType(unsigned int type)
    {
        switch (type)
        {
        case GL_FLOAT:
            return 4;
            break;
        default:
            std::__throw_out_of_range("Unknown type in BufferAttributes' GetSizeOfType");
            break;
        }
    }
};

class VertexBufferLayout
{
private:
    std::vector<BufferAttributes> _attributes;
    unsigned long _stride;

public:
    VertexBufferLayout() : _stride(0){};

    template <typename T>
    void Push(int count);
    const std::vector<BufferAttributes> GetAttributes() const
    {
        return _attributes;
    }

    unsigned long GetStride() const;
};
