#include "cameraComponent.hpp"
#include "context.hpp"
#include "renderer.hpp"
#include <iostream>

void CameraComponent::Update()
{
    SceneComponent::Update();
    GetUpVector();
    view = glm::lookAt(transform.position, targetPosition, up);
    float aspect = Context::GetContext()->renderer->GetAspect();
    projection = glm::perspective(glm::radians(cameraParameter.fov), aspect, cameraParameter.near, cameraParameter.far);
    //up = glm::vec3(0.0f, 1.0f, 0.0f);
    //if(glm::cos(glm::radians(transform.rotation.y))<= 0.0f)
    //    up = glm::vec3(0.0f, -1.0f, 0.0f);
}

void CameraComponent::SetPosition(glm::vec3 p)
{
    glm::vec3 dir = glm::normalize(targetPosition - p);
    float pitch = glm::atan(-dir.z, dir.x);
    float yaw = glm::asin(-dir.y);
    float roll  = 0.0f;
    glm::vec3 rotation = glm::vec3(glm::degrees(pitch), glm::degrees(yaw), glm::degrees(roll));
    transform.position = p;
    transform.rotation = rotation;
    distance = glm::distance(targetPosition, transform.position);
}

void CameraComponent::SetCameraRotation()
{
    glm::vec3 offset;
    float radYaw = glm::radians(transform.rotation.x);
    float radPitch = glm::radians(transform.rotation.y);
    offset.x = glm::cos(radYaw) * glm::cos(radPitch);
    offset.y = glm::sin(radPitch);
    offset.z = glm::sin(radYaw) * glm::cos(radPitch);
    glm::vec3 pos = targetPosition + (glm::normalize(offset) * distance);
    transform.position = pos;
}

void CameraComponent::GetUpVector()
{
    float radYaw = glm::radians(transform.rotation.x);
    float radPitch = glm::radians(transform.rotation.y+90.0f);
    up.x = cos(radYaw) * cos(radPitch);
    up.y = sin(radPitch);
    up.z = sin(radYaw) * cos(radPitch);
    up = glm::normalize(up);
}