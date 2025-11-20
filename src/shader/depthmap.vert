#version 430 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexcoord;

out vec2 fragTexcoord;

void main()
{
    fragTexcoord = vTexcoord;
    gl_Position = vec4(vPosition, 1.0f);
}
