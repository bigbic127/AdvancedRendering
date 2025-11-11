#pragma once
#include "sceneComponent.hpp"
#include <glm/glm.hpp>

struct CameraParameter
{
    float fov = 45.0f;
    float near = 0.01f;
    float far = 1000.0f; 
};

class CameraComponent:public SceneComponent
{
    public:
        glm::mat4 GetViewMatrix() const{return view;}
        glm::mat4 GetProjectionMatrix() const{return projection;}
        const CameraParameter& GetCameraParameter() const{return cameraParameter;}
        void SetCmaeraParameter(CameraParameter parameter){cameraParameter = parameter;}
        void SetFov(float fov){cameraParameter.fov = fov;}
        void SetNear(float near){cameraParameter.near = near;}
        void SetFar(float far){cameraParameter.far = far;}
        void Update() override; 
    private:
        glm::mat4 view{1.0f};
        glm::mat4 projection{1.0f};
        glm::vec3 targetPosition{0.0f, 2.0f, 0.0f};
        CameraParameter cameraParameter;
};
