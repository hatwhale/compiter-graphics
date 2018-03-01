#version 330

smooth in vec2 vTexCoord;
smooth in vec3 vNormal;
smooth in vec4 vEyeSpacePos;
smooth in vec3 vWorldPos;
out vec4 outputColor;

uniform sampler2D gSampler;
uniform vec4 vColor;

#include "dirLight.frag"

uniform DirectionalLight sunLight[iLightCount];
uniform vec3 vEyePosition;

uniform Material matActive;

#include "shadows.frag"

smooth in vec4 ShadowCoord[iLightCount];
uniform sampler2DArray shadowMap;


void main()
{
	vec4 vTexColor = texture2D(gSampler, vTexCoord);
	vec4 vMixedColor = vTexColor*vColor;
  
  vec3 vNormalized = normalize(vNormal);

  int l_i = 0;
  for(int l_i = 0; l_i < iLightCount; l_i++)
  {
	vec4 vShadowCoord = ShadowCoord[l_i];
	vShadowCoord /= vShadowCoord.w; vShadowCoord.q = l_i;
	float visibility = GetVisibility(shadowMap, vShadowCoord);

	vec4 vDiffuseColor = GetDirectionalLightColor(sunLight[l_i], vNormalized, visibility);
	vec4 vSpecularColor = GetSpecularColor(vWorldPos, vEyePosition, matActive, sunLight[l_i], vNormalized, visibility);
  }

  outputColor = vMixedColor * (vDiffuseColor + vSpecularColor);
}