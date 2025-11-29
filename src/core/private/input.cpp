#include "input.hpp"
#include "cameraComponent.hpp"
#include "context.hpp"
#include "world.hpp"
#include <imgui.h>

void Input::InputUpdate()
{

}

void Input::InputCameraUpdate()
{
    ImVec2 sceneSize = ImGui::GetContentRegionAvail();
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImVec2 scenePos = ImGui::GetWindowPos();
    ImGuiIO& io = ImGui::GetIO();

    CameraComponent* cameraComponent = Context::GetContext()->world->GetCurrentCamera()->GetComponent<CameraComponent>();
    Transform transform = cameraComponent->GetTransform();
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
            distance -= wheel * deletaTime * 0.1f;
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
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            isRotation = true;
        }
        else if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
            isRotation = false;            
            io.MousePos = mousePos;
            io.WantSetMousePos = true;
        }

        if(isLeftDragging)
        {
            transform.rotation.x += delta.x * deletaTime * 0.04f;
            transform.rotation.y += delta.y * deletaTime * 0.04f;
            cameraComponent->SetRotation(transform.rotation);
            cameraComponent->SetCameraRotation();
        }

    }else
    {
        if(ImGui::IsMouseReleased(ImGuiMouseButton_Left))
            ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);
    }

    if(isRotation && isLeftDragging)
    {
        if(scenePos.x>=pos.x)
        {
            io.MousePos.x = sceneSize.x+scenePos.x-5.0f;
            io.WantSetMousePos = true;
            delta.x = 0.0f;
        }
        else if(sceneSize.x+scenePos.x<=pos.x)
        {
            io.MousePos.x = scenePos.x+5.0f;
            io.WantSetMousePos = true;
            delta.x = 0.0f;
        }
        if(scenePos.y>=pos.y)
        {
            io.MousePos.y = sceneSize.y+scenePos.y-5.0f;
            io.WantSetMousePos = true;
            delta.y = 0.0f;
        }
        else if(sceneSize.y+scenePos.y<=pos.y)
        {
            io.MousePos.y = scenePos.y+5.0f;
            io.WantSetMousePos = true;
            delta.y = 0.0f;
        }
    }
}