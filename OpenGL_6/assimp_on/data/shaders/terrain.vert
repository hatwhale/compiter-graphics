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

smooth out vec2 vTexCoord;
smooth out vec3 vNormal;
smooth out vec3 vWorldPos;
smooth out vec4 vEyeSpacePos;

uniform mat4 HeightmapScaleMatrix;

uniform mat4 DepthBiasMVP;
smooth out vec4 ShadowCoord[LIGHT_MAX];

void main()
{
  vec4 inPositionScaled = HeightmapScaleMatrix*vec4(inPosition, 1.0);
  mat4 mMVP = matrices.projMatrix*matrices.viewMatrix*matrices.modelMatrix;
   
  gl_Position = mMVP*inPositionScaled;
  vEyeSpacePos = matrices.viewMatrix*matrices.modelMatrix*vec4(inPosition, 1.0);
  
  vTexCoord = inCoord;
  vNormal = inNormal;
   
  vec4 vWorldPosLocal = matrices.modelMatrix*inPositionScaled;
  vWorldPos = vWorldPosLocal.xyz;
  
  for(int l_i = 0; l_i < min(iLightCount, LIGHT_MAX); l_i++)
    ShadowCoord[l_i] = matrices.depthBiasMVP[l_i] * vWorldPosLocal;
}