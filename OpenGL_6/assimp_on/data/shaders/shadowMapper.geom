#version 330 core

#include "dirLight.frag"

uniform mat4 depthMV[iLightCount];

layout(triangles) in;
layout(triangle_strip, max_vertices = 30) out;
 
void main()
{
    for(int l_i = 0; l_i < iLightCount; l_i++)
	{
        gl_Layer = l_i;
        for(int i = 0; i < gl_in.length(); i++)
		{
            gl_Position = depthMV[l_i]*gl_in[i].gl_Position;
            EmitVertex();
        }
        EndPrimitive();
    }
}