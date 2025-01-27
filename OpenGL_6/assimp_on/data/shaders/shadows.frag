#version 330

#include_part

float GetVisibility(sampler2DArray shadowMap, vec4 vShadowCoord);

#definition_part

uniform sampler2D rotationTexture;

uniform float scale, radius;
uniform bool bShadowsOn;

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

float GetVisibility(sampler2DArray shadowMap, vec4 vShadowCoord)
{
    float visibility = 1.0;
    
    if(bShadowsOn)
    {
		visibility = 0.0;

		float bias = 0.005;

		vec4 srv = (texture(rotationTexture, gl_FragCoord.st * scale) * 2.0 - 1.0) * radius;

		mat2x2 srm = mat2x2(srv.xy, srv.zw);

		for(int i = 0; i < 16; i++)
		{
			float depth = texture(shadowMap, vShadowCoord.stq + vec3(srm * poissonDisk[i], 0.0)).r; 
		
			if(vShadowCoord.z - bias < depth)
			{
				visibility += 1.0;
			}
		}
		
		visibility /= 16.0;
	}
	
    return visibility;
}