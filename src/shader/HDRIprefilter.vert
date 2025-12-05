#version 430 core
layout(location = 0) in vec3 vPosition;

uniform mat4 mView;
uniform mat4 mProjection;

out vec3 fragPos;

void main()
{
    vec4 position = mProjection * mView * vec4(vPosition, 1.0f);
    fragPos = vPosition;
    gl_Position = position;
}