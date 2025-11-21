#version 430 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexcoord;
layout(location = 3) in vec3 vTangent;
layout(location = 4) in vec3 vBitangent;
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
    vec4 fragLightPosition;
    vec3 fragTangentLightPosition;
    vec3 fragTangentCameraPosition;
    vec3 fragTangentPosition;
    mat3 TBN;
} outFrag;
//light
uniform mat4 mLightMatrix;
//tangent space
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

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
    outFrag.fragLightPosition = mLightMatrix * vec4(outFrag.fragPosition, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(mModel)));
    vec3 T = normalize(normalMatrix * vTangent);
    vec3 N = normalize(normalMatrix * vNormal);
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);

    mat3 TBN = transpose(mat3(T, B, N));
    outFrag.fragTangentLightPosition = TBN * lightPosition;
    outFrag.fragTangentCameraPosition  = TBN * cameraPosition;
    outFrag.fragTangentPosition  = TBN * outFrag.fragPosition;
    outFrag.TBN = TBN;
    gl_Position = mProjection * mView * mModel * vec4(position, 1.0f);
}