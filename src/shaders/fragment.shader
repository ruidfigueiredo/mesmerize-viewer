#version 330 core

in vec2 textureCoordinate;
uniform sampler2D oddTextureSlot;
uniform sampler2D evenTextureSlot;
uniform float blendValue;

out vec4 color;

void main() 
{
    vec4 colorEven = texture(oddTextureSlot, textureCoordinate);
    vec4 colorOdd = texture(evenTextureSlot, textureCoordinate);
    color = mix(colorEven, colorOdd, blendValue);
}