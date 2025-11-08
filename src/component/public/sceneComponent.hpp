#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

struct Transform
{
    glm::vec3 position{0.0f, 0.0f, 0.0f};
    glm::vec3 rotation{0.0f, 0.0f, 0.0f};
    glm::vec3 scale{1.0f, 1.0f, 1.0f};
};

class IComponent
{
    public:
        virtual ~IComponent() = default;
        virtual void Update() = 0;
        virtual void Draw() = 0;
};

class SceneComponent:public IComponent
{
    public:
        void SetPosition(glm::vec3 p){transform.position = p;}
        void SetRotation(glm::vec3 r){transform.rotation = r;}
        void SetScale(glm::vec3 s){transform.scale = s;}
        glm::mat4 GetMatrix();
        void Update() override;
    protected:
        glm::quat ToQuaternion();
        Transform transform;
};