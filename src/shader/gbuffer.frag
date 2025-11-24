#version 430 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gDiffuseSpec;

in OutFrag
{
    vec3 FragPos;
    vec2 FragTexcoord;
    vec3 FragNor;
}inFrag;

//texture
uniform bool bDiffuse = false;
uniform bool bRoughness = false;
uniform bool bMetallic = false;
uniform bool bNormal = false;
uniform bool bAo = false;
uniform bool bDisp = false;
uniform sampler2D diffuseTexture;
uniform sampler2D roughnessTexture;
uniform sampler2D metallicTexture;
uniform sampler2D normalTexture;
uniform sampler2D aoTexture;
uniform sampler2D dispTexture;

void main
{
    vec3 diffuse = vec3(1.0f);
    float specular = 0.0f;
    if(bDiffuse)
    {
        diffuse = texture(diffuseTexture, inFrag.FragTexcoord).rgb;
    }
    if(bRoughness)
    {
        specular = texture(roughnessTexture, inFrag.FragTexcoord).r;
    }
    gPosition = inFrag.FragPos
    gNormal = normal(inFrag.FragNor);
    gDiffuseSpec = vec4(diffuse, specular);
}