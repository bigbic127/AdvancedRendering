#version 430 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexcoord;

layout(std140) uniform mCamera
{
    mat4 mView;
    mat4 mProjection;
};
uniform mat4 mModel;

out OutFrag
{
    vec3 FragPos;
    vec2 FragTexcoord;
    vec3 FragNor;
}outFrag;

void main()
{
    vec4 modePos = mModel * vec4(vPosition, 1.0f);
    outFrag.FragPos = modePos.xyz;
    outFrag.FragTexcoord = vTexcoord;
    outFrag.FragNor = transpose(inverse(mat3(mModel))) * vNormal;
    gl_Position = mProjection * mView * modePos;
}