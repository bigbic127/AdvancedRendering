#pragma once

class Window;
class IRenderer;
class Editor;
class Context
{
    public:
        Context(Window* w, IRenderer* r, Editor* e):window(w), renderer(r), editor(e){}
        IRenderer* renderer;
        Window* window;
        Editor* editor;        
};