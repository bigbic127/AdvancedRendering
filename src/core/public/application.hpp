#pragma once
#include "window.hpp"
#include "renderer.hpp"
#include "world.hpp"
#include "resourceManager.hpp"
#include <memory>

class Application
{
    public:
        Application();
        ~Application();
        void Init();
        void Loop();
    private:
        Window window;
        std::unique_ptr<Context> context;
        std::unique_ptr<IRenderer> renderer;
        std::unique_ptr<World> world;
        std::unique_ptr<ResourceManager> resourceManager;
};