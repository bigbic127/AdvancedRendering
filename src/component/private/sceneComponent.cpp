#include "sceneComponent.hpp"
#include <iostream>

glm::vec3 SceneComponent::GetDirection()
{
    glm::mat4 rotX = glm::rotate(glm::mat4(1.0f), glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 rotY = glm::rotate(glm::mat4(1.0f), glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotZ = glm::rotate(glm::mat4(1.0f), glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    glm::mat4 rotMat = rotZ * rotX * rotY;
    glm::vec3 dir = glm::vec3(rotMat * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    SetPosition(dir*5.0f);
    return dir;
}

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