#version 300 es

in mediump vec2 textureCoordinate;
uniform sampler2D textureSlot;
uniform mediump float blendValue;

out mediump vec4 color;

void main() 
{
    mediump vec4 textureColor = texture(textureSlot, textureCoordinate);
    textureColor.a = blendValue;
    color = textureColor;
}