#pragma once
#include "sceneComponent.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
        void SetIntensity(float intensity){lightIntensity = intensity;}
        void SetColor(glm::vec3 color){this->color = color;}
        float* GetIntensity(){return &lightIntensity;}
        float* GetColor(){return glm::value_ptr(color);}
    private:
        float lightIntensity = 1.0f;
        glm::vec3 color{1.0f};
};

class DirectionalLightComponent:public LightComponent
{
    private:
        LightType type = LightType::Directional;
};