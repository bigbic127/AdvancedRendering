#version 430 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gRoughness;
layout (location = 4) out vec4 gAmbientOcclusion;

in OutFrag
{
    vec3 FragPos;
    vec2 FragTexcoord;
    vec3 FragNor;
    mat3 TBN;
}inFrag;

//texture
uniform bool bDiffuse = false;
uniform bool bRoughness = false;
uniform bool bMetallic = false;
uniform bool bNormal = false;
uniform bool bAo = false;
uniform sampler2D diffuseTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform sampler2D normalTexture;
uniform sampler2D aoTexture;
void main()
{
    vec3 diffuse = vec3(1.0f);
    float roughness = 1.0f;
    vec3 normal = normalize(inFrag.FragNor);
    float occlusion = 1.0f;
    if(bDiffuse)
    {
        diffuse = texture(diffuseTexture, inFrag.FragTexcoord).rgb;
        diffuse = pow(diffuse, vec3(2.2));
    }
    if(bRoughness)
    {
        roughness = texture(roughnessTexture, inFrag.FragTexcoord).r;
    }
    if(bNormal)
    {
        normal = texture(normalTexture, inFrag.FragTexcoord).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(inFrag.TBN * normal);
    }
    if(bAo)
    {
        occlusion = texture(aoTexture, inFrag.FragTexcoord).r;
    }
    gPosition = vec4(inFrag.FragPos,1.0f);
    gNormal = vec4(normal, 1.0);
    gDiffuse = vec4(diffuse, 1.0);
    gRoughness = vec4(vec3(roughness,0,0), 1.0f);
    gAmbientOcclusion = vec4(vec3(occlusion,0,0), 1.0f);
}