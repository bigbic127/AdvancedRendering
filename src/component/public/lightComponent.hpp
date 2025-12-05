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
        ITexture* GetHDRITexture() const{return skyboxTexture;}
        ITexture* GetSkyboxTexture() const{return skyboxTexture;}
        glm::mat4 GetViewMatrix() const{return view;}
        glm::mat4 GetProjectionMatrix() const{return projection;}
        void Draw() override;
        void Update() override;
    private:
        float lightIntensity = 1.0f;
        glm::vec3 color{1.0f};
        glm::vec3 ambient{0.0f};
        IMesh* skyboxMesh;
        IShader* skyboxShader;
        ITexture* skyboxTexture;
        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};
        glm::vec3 targetPosition{0.0f, 0.0f, 0.0f};
};

class DirectionalLightComponent:public LightComponent
{
    private:
        LightType type = LightType::Directional;
};