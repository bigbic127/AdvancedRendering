#include "input.hpp"
#include "cameraComponent.hpp"
#include "context.hpp"
#include "world.hpp"
#include <imgui.h>
#include <iostream>

void Input::InputUpdate()
{

}

void Input::InputCameraUpdate()
{
    CameraComponent* cameraComponent = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>();
    Transform transform = cameraComponent->GetTransform();

    ImVec2 winSize = ImGui::GetWindowSize();
    ImVec2 contentMin = ImGui::GetWindowContentRegionMin();
    ImVec2 contentMax = ImGui::GetWindowContentRegionMax();
    ImVec2 sceneSize = { contentMax.x - contentMin.x, contentMax.y - contentMin.y };

    ImVec2 scenePos = ImGui::GetWindowPos();
    ImGuiIO& io = ImGui::GetIO();
    ImVec2 pos = ImGui::GetMousePos();
    ImVec2 delta = io.MouseDelta;
    float deletaTime = Context::GetContext()->world->GetDeletaTime();
    bool isHovered = ImGui::IsItemHovered();
    bool isLeftDragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
    float wheel = io.MouseWheel;

    if(isHovered)
    {
        //zoom
        if(wheel != 0.0f)
        {
            float distance = cameraComponent->GetDistance();
            distance -= wheel * deletaTime * 0.01f;
            distance = glm::clamp(distance, 5.0f, 50.0f);
            cameraComponent->SetDistance(distance);
            cameraComponent->SetCameraRotation();
        }
        //rotation
        if(ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            mousePos = ImGui::GetMousePos();
            isRotation = false;
        }else if(isLeftDragging)
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_None);
            isRotation = true;
        }
        else if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            isRotation = false;            
            io.MousePos = mousePos;
            io.WantSetMousePos = true;
        }
    }else
    {
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }
    if(isRotation && isLeftDragging)
    {
        if(scenePos.x+2.0f>=pos.x)
        {
            io.MousePos.x = sceneSize.x+scenePos.x-5.0f;
            io.WantSetMousePos = true;
            delta.x = -1.0f;
        }
        else if(sceneSize.x+scenePos.x-2.0f<=pos.x)
        {
            io.MousePos.x = scenePos.x+5.0f;
            io.WantSetMousePos = true;
            delta.x = 1.0f;
        }
        if(scenePos.y+2.0f>=pos.y)
        {
            io.MousePos.y = sceneSize.y+scenePos.y-5.0f;
            io.WantSetMousePos = true;
            delta.y = -1.0f;
        }
        else if(sceneSize.y+scenePos.y-2.0f <=pos.y)
        {
            io.MousePos.y = scenePos.y+5.0f;
            io.WantSetMousePos = true;
            delta.y = 1.0f;
        }
        if(isLeftDragging)
        {
            transform.rotation.x += delta.x * deletaTime * 0.04f;
            transform.rotation.y += delta.y * deletaTime * 0.04f;
            cameraComponent->SetRotation(transform.rotation);
            cameraComponent->SetCameraRotation();
        }
    }
}