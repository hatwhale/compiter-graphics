#version 330

uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;                                                                           
	mat4 normalMatrix;
} matrices;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

smooth out vec3 vNormal;
smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;
smooth out vec4 vEyeSpacePos;
void main()
{
	gl_Position = matrices.projMatrix*matrices.viewMatrix*matrices.modelMatrix*vec4(inPosition, 1.0);
	vTexCoord = inCoord;
	vec4 vRes = matrices.normalMatrix*vec4(inNormal, 0.0);
	vNormal = vRes.xyz;	
    vec4 vWorldPosVertex = matrices.modelMatrix*vec4(inPosition, 1.0);
	vWorldPos = vWorldPosVertex.xyz;
	vEyeSpacePos = matrices.viewMatrix*matrices.modelMatrix*vec4(inPosition, 1.0);
}