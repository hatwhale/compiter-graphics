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

smooth out vec2 vTexCoord;

void main()
{
  mat4 mMV = matrices.viewMatrix*matrices.modelMatrix;  
  mat4 mMVP = matrices.projMatrix*matrices.viewMatrix*matrices.modelMatrix;
  
  vTexCoord = inCoord;

	gl_Position = mMVP*vec4(inPosition, 1.0);
}