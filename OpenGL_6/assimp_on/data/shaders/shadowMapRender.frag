#version 330

out vec4 outputColor;

uniform sampler2DArray shadowMap;

smooth in vec2 vCoord;
uniform int layer;

void main()
{
	float depth = texture(shadowMap, vec3(vCoord, layer)).r;

	outputColor = vec4(depth, depth, depth, 1.0);
}
