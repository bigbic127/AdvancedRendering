#version 430 core
layout(location = 0) in vec3 vPosition;
uniform mat4 mView, mProjection;
out vec3 fragTexcoord;

void main()
{
    vec4 position = mProjection * mView * vec4(vPosition, 1.0f);
    fragTexcoord = vPosition;
    gl_Position = position.xyww;
}
