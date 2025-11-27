#version 430 core
out vec4 FragColor;
in vec3 fragTexcoord;

uniform samplerCube skybox;

void main()
{
    vec3 color = texture(skybox, fragTexcoord).rgb;
    FragColor = vec4(color, 1.0);
}