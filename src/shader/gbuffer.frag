#version 430 core
layout (location = 0) out vec4 gPosition;
layout (location = 1) out vec4 gNormal;
layout (location = 2) out vec4 gDiffuse;
layout (location = 3) out vec4 gORMI;
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
//material parameter
uniform vec3 diffuseColor = vec3(1.0f);

//pbr parameter
uniform float roughnessFactor = 0.5f;
uniform float metallicFactor = 0.0f;

void main()
{
    vec3 diffuse = vec3(1.0f);
    float roughness = 1.0f;
    vec3 normal = normalize(inFrag.FragNor);
    float occlusion = 1.0f;
    float metallic = 1.0f;
    if(bDiffuse)
    {
        diffuse = texture(diffuseTexture, inFrag.FragTexcoord).rgb;
        diffuse = pow(diffuse, vec3(2.2));
    }
    if(bRoughness)
        roughness = texture(roughnessTexture, inFrag.FragTexcoord).r;
    if(bMetallic)
        metallic = texture(metallicTexture, inFrag.FragTexcoord).r;
    if(bNormal)
    {
        normal = texture(normalTexture, inFrag.FragTexcoord).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(inFrag.TBN * normal);
    }
    if(bAo)
        occlusion = texture(aoTexture, inFrag.FragTexcoord).r;
    //result
    diffuse = diffuse*diffuseColor;
    roughness = roughness*roughnessFactor;
    metallic = metallic*metallicFactor;
    vec4 ORMI = vec4(occlusion, roughness, metallic, 1.0f);

    gPosition = vec4(inFrag.FragPos,1.0f);
    gNormal = vec4(normal, 1.0);
    gDiffuse = vec4(diffuse, 1.0);
    gORMI = ORMI;
}