#version 300 es

in mediump vec2 textureCoordinate;
uniform sampler2D oddTextureSlot;
uniform sampler2D evenTextureSlot;
uniform mediump float blendValue;

out mediump vec4 color;

void main() 
{
    mediump vec4 colorEven = texture(oddTextureSlot, textureCoordinate);
    mediump vec4 colorOdd = texture(evenTextureSlot, textureCoordinate);
    color = mix(colorEven, colorOdd, blendValue);
}