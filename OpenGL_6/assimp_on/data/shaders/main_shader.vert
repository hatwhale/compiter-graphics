#version 330

#include "dirLight.frag"

uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelMatrix;
	mat4 viewMatrix;                                                                           
	mat4 normalMatrix;
	mat4 depthBiasMVP[LIGHT_MAX];
} matrices;

uniform int iLightCount;

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

smooth out vec3 vNormal;
smooth out vec2 vTexCoord;
smooth out vec3 vWorldPos;

smooth out vec4 vEyeSpacePos;
smooth out vec4 ShadowCoord[LIGHT_MAX];

void main()
{
  mat4 mMV = matrices.viewMatrix*matrices.modelMatrix;  
  mat4 mMVP = matrices.projMatrix*matrices.viewMatrix*matrices.modelMatrix;
  
  vTexCoord = inCoord;

  vEyeSpacePos = mMV*vec4(inPosition, 1.0);
	gl_Position = mMVP*vec4(inPosition, 1.0);

  vNormal = (matrices.normalMatrix*vec4(inNormal, 1.0)).xyz;
  vWorldPos = (matrices.modelMatrix*vec4(inPosition, 1.0)).xyz;
  
  for(int l_i = 0; l_i < min(iLightCount, LIGHT_MAX); l_i++)
    ShadowCoord[l_i] = matrices.depthBiasMVP[l_i] * vec4(vWorldPos, 1.0);
}