#pragma once
#include <glm/glm.hpp>

class Texture;

struct MaterialParameter
{
    //color
    glm::vec3 ambientColor = glm::vec3(0.0f);
    glm::vec3 diffuseColor = glm::vec3(1.0f);
    glm::vec3 specularColor = glm::vec3(0.5f);
    //texture
    Texture* diffuseTexture = nullptr;
    Texture* roughnessTextrue = nullptr;
    Texture* metallicTexture = nullptr;
    Texture* normalTexture = nullptr;
    //properties
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.5f;
    float shininess = 0.5f;
    float specularShininess = 32.0f;
};

class IMaterial
{

};

class OpenGLMaterial
{

};