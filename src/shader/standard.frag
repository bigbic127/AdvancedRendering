#version 430 core

out vec4 FragColor;
in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexcoord;

void main()
{
    FragColor = vec4(vec3(1.0f), 1.0f);
}