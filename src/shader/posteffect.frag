#version 430 core
out vec4 FragColor;

in vec2 fragTexcoord;

uniform sampler2D screenTexture;

void main()
{
    vec3 color = texture(screenTexture, fragTexcoord).rgb;
    //color = (1- color);
    //gray
    //float average = (color.x + color.y + color.z) / 3.0;
    //color = vec3(average, average, average);
    FragColor = vec4(color, 1.0);
}