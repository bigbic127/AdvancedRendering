#version 430 core
out vec4 FragColor;
in vec3 fragTexcoord;

uniform samplerCube skybox;

void main()
{
    vec3 color = texture(skybox, fragTexcoord).rgb;
    color = pow(color, vec3(2.2f));
    FragColor = vec4(vec3(1.0f), 1.0f);
}