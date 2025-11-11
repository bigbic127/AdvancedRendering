#include "window.hpp"
#include "logger.hpp"

Window::~Window()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Window::Init()
{
    if(!glfwInit())
        return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif
    window = glfwCreateWindow(width, height, "Advanced Rendering", nullptr, nullptr);
    if(window == nullptr)
    {
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSetWindowUserPointer(window, this);
    glfwSetErrorCallback(Logger::WindowErrorCallback);
    glfwSetWindowSizeCallback(window, Window::WindowResizeCallback);
    if(!gladLoadGLLoader(GLADloadproc(glfwGetProcAddress)))
        return false;
    glfwSwapInterval(1);
    //editor
    editor.Init();
    return true;
}

void Window::WindowResizeCallback(GLFWwindow* window, int w, int h)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->width = w;
    win->height = h;
}

bool Window::ShouldClose()
{
    return glfwWindowShouldClose(window);
}

void Window::Update()
{
    editor.Update();
    glfwSwapBuffers(window);
    glfwPollEvents();
}