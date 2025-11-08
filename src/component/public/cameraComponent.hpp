#pragma once
#include "sceneComponent.hpp"
#include <glm/glm.hpp>

struct CameraParameter
{
    float fov = 45.0f;
    float aspect = 1.778f;
    float near = 0.01f;
    float far = 1000.0f; 
};

class CameraComponent:public SceneComponent
{
    public:
        void Update() override; 
    private:
        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};
        glm::vec3 targetPosition{0.0f};
        CameraParameter cameraParameter;
};
