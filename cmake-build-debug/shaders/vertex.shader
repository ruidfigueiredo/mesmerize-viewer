#version 300 es

layout(location = 0) in vec4 position;
layout(location = 1) in vec2 _textureCoordinate;

uniform mat4 mvp;

out vec2 textureCoordinate;

void main() 
{
    gl_Position = mvp * position;
    textureCoordinate = _textureCoordinate;
}