#version 430 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexcoord;
layout(location = 3) in vec3 tangent;
layout(location = 4) in vec3 biTangent;
layout(std140) uniform mCamera
{
    mat4 mView;
    mat4 mProjection;
};
uniform mat4 mModel;
uniform bool bStencil = false;
uniform float stencilOutline = 0.1f;
//output
out OutFrag
{
    vec3 fragPosition;
    vec3 fragNormal;
    vec2 fragTexcoord;
} outFrag;

void main()
{
    vec3 position = vPosition;
    if (bStencil)
    {
        position += (vNormal*stencilOutline);
    }
    outFrag.fragPosition = vec3(mModel * vec4(position, 1.0f));
    outFrag.fragNormal = transpose(inverse(mat3(mModel))) * vNormal;
    outFrag.fragTexcoord = vTexcoord;
    gl_Position = mProjection * mView * mModel * vec4(position, 1.0f);
}
