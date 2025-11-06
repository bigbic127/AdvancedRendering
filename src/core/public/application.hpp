#pragma once
#include "window.hpp"
#include "renderer.hpp"
#include <memory>

class Application
{
    public:
        Application();
        ~Application();
        void Loop();
    private:
        Window window;
        std::unique_ptr<Context> context;
        std::unique_ptr<IRenderer> renderer;
};