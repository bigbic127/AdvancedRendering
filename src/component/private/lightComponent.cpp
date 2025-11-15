#include "lightComponent.hpp"
#include "mesh.hpp"
#include <string>
#include "shader.hpp"

LightComponent::LightComponent()
{
    skyboxMesh = std::make_unique<CubeMesh>();
    const std::string vsPath = "/shader/skybox.vert";
    const std::string fsPath = "/shader/skybox.frag";
    skyboxShader = std::make_unique<OpenGLShader>(vsPath, fsPath);
    static_cast<OpenGLShader*>(skyboxShader.get())->SetInt("skybox", 0);
    glActiveTexture(GL_TEXTURE0);
}
