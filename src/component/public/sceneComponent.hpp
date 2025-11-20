#pragma once
#include "component.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform
{
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f};
    glm::vec3 scale{1.0f};
};

class SceneComponent:public IComponent
{
    public:
        void SetTransform(Transform transform){this->transform = transform;}
        void SetPosition(glm::vec3 p){transform.position = p;}
        void SetRotation(glm::vec3 r){transform.rotation = r;}
        void SetScale(glm::vec3 s){transform.scale = s;}
        const Transform& GetTransform()const{return transform;}
        float* GetRotation(){return glm::value_ptr(transform.rotation);}
        glm::vec3 GetDirection();
        glm::mat4 GetMatrix();
        void Update() override;
        void Draw() override;
    protected:
        glm::quat ToQuaternion();
        Transform transform;
};