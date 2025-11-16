#pragma once
#include "sceneComponent.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

class IMesh;
class IShader;
class ITexture;

enum LightType
{
    Directional,
    Spot,
    Omni,
    Plane
};

class LightComponent:public SceneComponent
{
    public:
        LightComponent();
        void SetIntensity(float intensity){lightIntensity = intensity;}
        void SetColor(glm::vec3 color){this->color = color;}
        float* GetIntensity(){return &lightIntensity;}
        float* GetColor(){return glm::value_ptr(color);}
        float* GetAmbient(){return glm::value_ptr(ambient);}
        ITexture* GetSkyboxTexture() const{return skyboxTexture;}
        void Draw() override;
    private:
        float lightIntensity = 1.0f;
        glm::vec3 color{1.0f};
        glm::vec3 ambient{0.0f};
        IMesh* skyboxMesh;
        IShader* skyboxShader;
        ITexture* skyboxTexture;
};

class DirectionalLightComponent:public LightComponent
{
    private:
        LightType type = LightType::Directional;
};