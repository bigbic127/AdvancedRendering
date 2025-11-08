#include "sceneComponent.hpp"

glm::mat4 SceneComponent::GetMatrix()
{
    glm::mat4 model(1.0f);
    model = glm::translate(model, transform.position);
    model *= glm::mat4_cast(ToQuaternion());
    model = glm::scale(model, transform.scale);
    return model;
}

glm::quat SceneComponent::ToQuaternion()
{
    return glm::normalize(glm::quat(glm::radians(transform.rotation)));
}

void SceneComponent::Update()
{

}

void SceneComponent::Draw()
{
    
}