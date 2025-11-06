#include "application.hpp"
#include "context.hpp"

Application::Application()
{
    if(window.Init())
    {
        renderer = std::make_unique<OpenGLRenderer>();
        context = std::make_unique<Context>(&window, renderer.get(), window.GetEditor());
        //Context Init
        window.GetEditor()->SetContext(context.get());        
        Loop();
    }
}

Application::~Application()
{

}

void Application::Loop()
{
    while(!window.ShouldClose())
    {
        renderer->Draw();
        window.Update();
    }
    
}

int main()
{
    Application application;
    return 0;
}

