#pragma once
#include <glm/glm.hpp>
#include "shader.hpp"

class ITexture;

struct MaterialParameter
{
    //color
    glm::vec3 ambientColor{0.0f};
    glm::vec3 diffuseColor{1.0f};
    glm::vec3 specularColor{0.5f};
    //texture
    ITexture* diffuseTexture = nullptr;
    ITexture* roughnessTextrue = nullptr;
    ITexture* metallicTexture = nullptr;
    ITexture* normalTexture = nullptr;
    //properties
    float metallicFactor = 0.0f;
    float roughnessFactor = 0.5f;
    float shininess = 0.5f;
    float specularShininess = 32.0f;
};

class IMaterial
{
    public:
        virtual ~IMaterial() = default;
        virtual void Draw() = 0;
};

class OpenGLMaterial:public IMaterial
{
    public:
        OpenGLMaterial(IShader* s):shader(dynamic_cast<OpenGLShader*>(s)){}
        void Draw() override;
    private:
        OpenGLShader* shader;

};