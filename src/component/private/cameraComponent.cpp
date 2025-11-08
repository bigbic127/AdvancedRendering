#include "cameraComponent.hpp"

void CameraComponent::Update()
{
    view = glm::lookAt(transform.position, targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
    projection = glm::perspective(glm::radians(cameraParameter.fov), cameraParameter.aspect, cameraParameter.near, cameraParameter.far);
}