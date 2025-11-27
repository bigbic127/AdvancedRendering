#version 430 core
layout(location = 0) in vec3 vPosition;
layout(location = 1) in vec3 vNormal;
layout(location = 2) in vec2 vTexcoord;
layout(location = 3) in vec3 vTangent;
layout(std140) uniform mCamera
{
    mat4 mView;
    mat4 mProjection;
};
out OutFrag
{
    vec3 FragPos;
    vec2 FragTexcoord;
    vec3 FragNor;
    mat3 TBN;
}outFrag;
uniform mat4 mModel;
uniform bool bDisp = false;
uniform sampler2D dispTexture;
uniform float heightScale = 0.1f;

void main()
{
    vec3 position = vPosition;
    if (bDisp)
    {
        float height = texture(dispTexture, vTexcoord).r;
        float displacement = (height - 0.5f) * heightScale;
        position += vNormal * displacement;
    }
    vec4 modelPos = mModel * vec4(position, 1.0f);
    //normal Tangent Space
    vec3 N = normalize(mat3(mModel) * vNormal);
    vec3 T = normalize(mat3(mModel) * vTangent);
    vec3 B = normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    outFrag.TBN = TBN;
    outFrag.FragPos = modelPos.xyz;
    outFrag.FragTexcoord = vTexcoord;
    outFrag.FragNor = transpose(inverse(mat3(mModel))) * vNormal;
    gl_Position = mProjection * mView * modelPos;
}