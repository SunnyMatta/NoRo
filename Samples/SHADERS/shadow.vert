#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 u_LightSpaceMatrix;
uniform mat4 model;

void main()
{
    // Transform the vertex into world space, then into light-view space
    gl_Position = u_LightSpaceMatrix * model * vec4(aPos, 1.0);
}
