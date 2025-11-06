#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "editor.hpp"

class Window
{
    public:
        Window(int w= 800, int h= 600):width(w),height(h){}
        ~Window();
        bool Init();
        bool ShouldClose();
        void Update();
        GLFWwindow* GetWindow()const{return window;}
        Editor* GetEditor(){return &editor;}
    private:
        GLFWwindow* window;
        int width, height;
        Editor editor;
};