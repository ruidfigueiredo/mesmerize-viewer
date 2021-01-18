#version 330 core

in vec2 textureCoordinate;
uniform sampler2D textureSlot;

out vec4 color;

void main() 
{
    color = texture(textureSlot, textureCoordinate);
}