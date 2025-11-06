#include "mesh.hpp"

OpenGLMesh::OpenGLMesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);//position
    glEnableVertexAttribArray(1);//normal
    glEnableVertexAttribArray(2);//texcoord
    glEnableVertexAttribArray(3);//tangent
    glEnableVertexAttribArray(4);//bitangent
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texcoord));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent));
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, bitangent));
    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    indiceSize = GLsizei(indices.size()*sizeof(unsigned int));
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indiceSize, indices.data(), GL_STATIC_DRAW);
    glBindVertexArray(0);
}

OpenGLMesh::~OpenGLMesh()
{
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ebo);
}

void OpenGLMesh::Bind()
{
    glBindVertexArray(vao);
}
void OpenGLMesh::UnBind()
{
    glBindVertexArray(0);
}

void OpenGLMesh::Draw()
{
    glDrawElements(GL_TRIANGLES, indiceSize, GL_UNSIGNED_INT, 0);
}
