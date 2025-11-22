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
} outFrag;
out OutTangent
{
    vec3 fragTangentLightPosition;
    vec3 fragTangentCameraPosition;
    vec3 fragTangentPosition;
    vec3 fragTangentDirection;
} outTangent;
//light
uniform mat4 mLightMatrix;
uniform vec3 directionalLight = vec3(0.0f, 1.0f, 0.0f);

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

    //normal Tangent Space
    mat3 normalMatrix = transpose(inverse(mat3(mModel)));
    vec3 N = normalize(normalMatrix * vNormal);
    vec3 T = normalize(mat3(mModel) * vTangent);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = transpose(mat3(T, B, N));
    outTangent.fragTangentLightPosition = TBN * lightPosition;
    outTangent.fragTangentCameraPosition  = TBN * cameraPosition;
    outTangent.fragTangentPosition  = TBN * outFrag.fragPosition;
    outTangent.fragTangentDirection = TBN * directionalLight;

    gl_Position = mProjection * mView * mModel * vec4(position, 1.0f);
}