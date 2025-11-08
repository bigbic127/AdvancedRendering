#pragma once

class Window;
class IRenderer;
class Editor;
class World;
class ResourceManager;

class Context
{
    public:
        Context(Window* w, IRenderer* r, Editor* e, World* wd, ResourceManager* rm):window(w), renderer(r), editor(e), world(wd), resourceManager(rm){instance = this;}
        static Context* GetContext(){return instance;}
        IRenderer* renderer;
        Window* window;
        Editor* editor;
        World* world;
        ResourceManager* resourceManager;
        private:
            static Context* instance;
        
};