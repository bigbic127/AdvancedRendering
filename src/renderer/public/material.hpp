#pragma once
#include <glm/glm.hpp>
#include "shader.hpp"

class ITexture;
class MeshComponent;

enum MaterialType
{
    Plastic
};

struct MaterialParameter
{
    //color
    glm::vec3 ambientColor{0.0f};
    glm::vec3 diffuseColor{0.77f};
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
        virtual void Bind() = 0;
        virtual void UnBind() = 0 ;
};

class OpenGLMaterial:public IMaterial
{
    public:
        OpenGLMaterial(IShader* s):shader(static_cast<OpenGLShader*>(s)){}
        void Bind() override;
        void Draw(MeshComponent* component);
        void UnBind() override;
        void AddTexture(ITexture* texture, int type);
        OpenGLShader* GetShader()const{return shader;}
        MaterialParameter* GetParameter(){return &parameter;}
    private:
        OpenGLShader* shader;
        MaterialParameter parameter;
};