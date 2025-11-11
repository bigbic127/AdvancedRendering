#pragma once
#include <GLFW/glfw3.h>
#include <imgui.h>

class Context;

struct NanumSquare
{
    ImFont* extraBold;
    ImFont* bold;
    ImFont* reqular;
    ImFont* light;
    ImFont* extraBold_ac;
    ImFont* bold_ac;
    ImFont* reqular_ac;
    ImFont* light_ac;
    float size;
};

class Editor
{
    public:
        ~Editor();
        void Init();
        void Update();
    private:
        void SetupStyle();
        void CreateLayout();
        void CreateMainMenuBar();
        void CreateMainToolBar();
        void CreateDockSpace();
        void CreateSceneLayout();
        void CreateRightPanel();
        void CreateStatus();
    private:
        int selectedDisplay = 0;
        int selectedShape = 0;
        NanumSquare nanumSquare;
        bool bInit = false;
};