#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 model;

void main()
{
    //transform the vertex into world space
    gl_Position = u_LightSpaceMatrix * model * vec4(aPos, 1.0);
}
