#version 430 core
layout(location = 0) in vec3 vPosition;
layout(std140) uniform mCamera
{
    mat4 mView;
    mat4 mProjection;
};
uniform mat4 mModel;

void main()
{
    vec4 position = mProjection * mView * mModel * vec4(vPosition, 1.0f);
    gl_Position = position;
}
