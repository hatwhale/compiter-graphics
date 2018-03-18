#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;
smooth in vec3 vWorldPos;
out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

#include "dirLight.frag"

uniform DirectionalLight sunLight[LIGHT_MAX];
uniform int iLightCount;
uniform vec3 vEyePosition;

uniform Material matActive;

#include "shadows.frag"

smooth in vec4 ShadowCoord[LIGHT_MAX];
uniform sampler2DArray shadowMap;


void main()
{
	vec4 vTexColor = texture2D(gSampler, vTexCoord);
	vec4 vMixedColor = vTexColor*vColor;
  
  vec3 vNormalized = normalize(vNormal);

  outputColor = vec4(0.0f);
  for(int l_i = 0; l_i < min(iLightCount, LIGHT_MAX); l_i++)
  {
	vec4 vShadowCoord = ShadowCoord[l_i];
	vShadowCoord /= vShadowCoord.w; vShadowCoord.q = l_i;
	float visibility = GetVisibility(shadowMap, vShadowCoord);
	if(sunLight[l_i].bSkybox)visibility = 0.0f;

	vec4 vDiffuseColor = GetDirectionalLightColor(sunLight[l_i], vNormalized, visibility);
	vec4 vSpecularColor = GetSpecularColor(vWorldPos, vEyePosition, matActive, sunLight[l_i], vNormalized, visibility);
	if(sunLight[l_i].bSwitch)outputColor += (vDiffuseColor + vSpecularColor);
  }
  outputColor *= vMixedColor;
}