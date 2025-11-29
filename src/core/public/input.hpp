#pragma once
#include <imgui.h>


class Input
{
    public:
        void InputUpdate();
        void InputCameraUpdate();
    private:
        bool isLeftClicekd = false;
        bool delay = false;
        ImVec2 mousePos;
        bool isRotation = false;
};