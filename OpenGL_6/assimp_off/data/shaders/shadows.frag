#version 330

#include_part

float GetVisibility(sampler2DShadow shadowMap, vec4 vShadowCoord);
//float GetVisibility(sampler2D shadowMap, vec4 vShadowCoord);

#definition_part

uniform sampler2D rotationTexture;

uniform float scale, radius;

uniform int bShadowsOn;

float PCF(in vec4 ShadowCoord, sampler2DShadow shadowMap)
{
  float res = 0.0;

  res += textureProjOffset(shadowMap, ShadowCoord, ivec2(-1,-1));
  res += textureProjOffset(shadowMap, ShadowCoord, ivec2( 0,-1));
  res += textureProjOffset(shadowMap, ShadowCoord, ivec2( 1,-1));
  res += textureProjOffset(shadowMap, ShadowCoord, ivec2(-1, 0));
  res += textureProjOffset(shadowMap, ShadowCoord, ivec2( 0, 0));
  res += textureProjOffset(shadowMap, ShadowCoord, ivec2( 1, 0));
  res += textureProjOffset(shadowMap, ShadowCoord, ivec2(-1, 1));
  res += textureProjOffset(shadowMap, ShadowCoord, ivec2( 0, 1));
  res += textureProjOffset(shadowMap, ShadowCoord, ivec2( 1, 1));

  return (res / 9.0);
}

vec2 poissonDisk[16] = vec2[]( 
   vec2( -0.94201624, -0.39906216 ), 
   vec2( 0.94558609, -0.76890725 ), 
   vec2( -0.094184101, -0.92938870 ), 
   vec2( 0.34495938, 0.29387760 ), 
   vec2( -0.91588581, 0.45771432 ), 
   vec2( -0.81544232, -0.87912464 ), 
   vec2( -0.38277543, 0.27676845 ), 
   vec2( 0.97484398, 0.75648379 ), 
   vec2( 0.44323325, -0.97511554 ), 
   vec2( 0.53742981, -0.47373420 ), 
   vec2( -0.26496911, -0.41893023 ), 
   vec2( 0.79197514, 0.19090188 ), 
   vec2( -0.24188840, 0.99706507 ), 
   vec2( -0.81409955, 0.91437590 ), 
   vec2( 0.19984126, 0.78641367 ), 
   vec2( 0.14383161, -0.14100790 ) 
);

float GetVisibility(sampler2DShadow shadowMap, vec4 vShadowCoord)
//float GetVisibility(sampler2D shadowMap, vec4 vShadowCoord)
{
    
    float visibility = 1.0;
    
    
    if(bShadowsOn == 1)
    {
      visibility = PCF(vShadowCoord, shadowMap);
      /*visibility = 0.0;

      float bias = 0.005;

      vShadowCoord /= vShadowCoord.w;

      vec4 srv = (texture(rotationTexture, gl_FragCoord.st * scale) * 2.0 - 1.0) * radius;

      mat2x2 srm = mat2x2(srv.xy, srv.zw);

      for(int i = 0; i < 16; i++)
      {
         float depth = texture(shadowMap, vShadowCoord.st + srm * poissonDisk[i]).r; 
    
         if(vShadowCoord.z - bias < depth)
         {
          visibility += 1.0;
         }
     }
    
     visibility /= 16.0;*/
     }
  return visibility;
}