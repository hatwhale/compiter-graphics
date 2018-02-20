#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;
smooth in vec3 vWorldPos;
out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

#include "dirLight.frag"
uniform DirectionalLight sunLight;

#include "shadows.frag"

smooth in vec4 ShadowCoord;
uniform sampler2DShadow  shadowMap;
//uniform sampler2D  shadowMap;

void main()
{
	vec3 vNormalized = normalize(vNormal);
	
	vec4 vTexColor = texture2D(gSampler, vTexCoord);
	//vec4 vDirLightColor = getDirectionalLightColor(sunLight, vNormal);
	vec4 vMixedColor = vTexColor*vColor;//*vDirLightColor;
	
	float visibility = GetVisibility(shadowMap, ShadowCoord);
	vec4 vDiffuseColor = getDirectionalLightColor(sunLight, vNormalized, visibility);
	//PCF(ShadowCoord);
	//textureProj(shadowMap, ShadowCoord);
	//GetVisibility(shadowMap, ShadowCoord);
	outputColor = vMixedColor*vDiffuseColor;//visibility;
}