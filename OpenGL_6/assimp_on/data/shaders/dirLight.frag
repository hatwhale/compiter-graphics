#version 330

#include_part

const int LIGHT_MAX = 10;

struct DirectionalLight
{
	vec3 vColor;
	vec3 vDirection;

	float fAmbient;
	bool bSkybox;

	bool bSwitch;
};

vec4 GetDirectionalLightColor(DirectionalLight dirLight, vec3 vNormal, float Shadow);

struct Material
{
   float fSpecularIntensity;
   float fSpecularPower;
};

vec4 GetSpecularColor(vec3 vWorldPos, vec3 vEyePos, Material mat, DirectionalLight dLight, vec3 vNormal, float Shadow);

#definition_part

vec4 GetDirectionalLightColor(DirectionalLight dirLight, vec3 vNormal, float Shadow)
{
	float fDiffuseIntensity = max(0.0, dot(vNormal, -dirLight.vDirection));
	float fMult = dirLight.fAmbient + fDiffuseIntensity * Shadow;
	return vec4(dirLight.vColor*fMult, 1.0);
}

vec4 GetSpecularColor(vec3 vWorldPos, vec3 vEyePos, Material mat, DirectionalLight dLight, vec3 vNormal, float Shadow)
{
   vec4 vResult = vec4(0.0, 0.0, 0.0, 0.0);
   
   vec3 vLightDir = dLight.bSkybox ? -dLight.vDirection : dLight.vDirection;
   vec3 vReflectedVector = normalize(reflect(vLightDir, normalize(vNormal)));
   vec3 vVertexToEyeVector = normalize(vEyePos-vWorldPos);
   float fSpecularFactor = dot(vVertexToEyeVector, vReflectedVector);

   fSpecularFactor = pow(fSpecularFactor, mat.fSpecularPower);
   
   if (fSpecularFactor > 0)
      vResult = vec4(dLight.vColor, 1.0) * mat.fSpecularIntensity * fSpecularFactor * Shadow;
   
   return vResult;
}