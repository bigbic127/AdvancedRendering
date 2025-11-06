#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texcoord;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class IMesh
{
    public:
        virtual ~IMesh() = default;
        virtual void Bind() = 0;
        virtual void Draw() = 0;
        virtual void UnBind() = 0;
};

class OpenGLMesh:public IMesh
{
    public:
        OpenGLMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
        ~OpenGLMesh();
        void Bind() override;
        void Draw() override;
        void UnBind() override;
    private:
        unsigned int vao, vbo, ebo;
        GLsizei indiceSize;
};