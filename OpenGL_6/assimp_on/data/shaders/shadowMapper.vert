#version 330 core

#include "dirLight.frag"

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;

// Values that stay constant for the whole mesh.
uniform mat4 mModel;

void main(){
	gl_Position = mModel*vec4(vertexPosition_modelspace,1);
}

