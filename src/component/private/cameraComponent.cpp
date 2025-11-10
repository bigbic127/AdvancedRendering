#include "cameraComponent.hpp"
#include "context.hpp"
#include "renderer.hpp"

void CameraComponent::Update()
{
    SceneComponent::Update();
    view = glm::lookAt(transform.position, targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));
    float aspect = Context::GetContext()->renderer->GetAspect();
    projection = glm::perspective(glm::radians(cameraParameter.fov), aspect, cameraParameter.near, cameraParameter.far);
}