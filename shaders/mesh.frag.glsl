#version 430

layout(set = 0, binding = 1) uniform sampler2D Texture1;

layout(location = 0) in  vec2 InUV;
layout(location = 0) out vec4 OutColor;

void main()
{
	OutColor = vec4(texture(Texture1, InUV).xyz, 1.0f);
}