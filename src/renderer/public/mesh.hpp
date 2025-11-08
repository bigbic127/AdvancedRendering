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
        OpenGLMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        ~OpenGLMesh();
        void Bind() override;
        void UnBind() override;
        void Draw() override;
    private:
        unsigned int vao, vbo, ebo;
        GLsizei indiceSize;
};

//standard Shape
class CubeMesh:public OpenGLMesh
{
    public:
        CubeMesh():OpenGLMesh(CreateVertices(),
                         CreateIndices()){}
        static std::vector<Vertex> CreateVertices()
        {
            std::vector<Vertex> vertices = {
                // Front face
                {{-1.0f, -1.0f,  1.0f},{0.0f,  0.0f,  1.0f},{0.0f, 0.0f}},
                {{ 1.0f, -1.0f,  1.0f},{0.0f,  0.0f,  1.0f},{1.0f, 0.0f}},
                {{ 1.0f,  1.0f,  1.0f},{0.0f,  0.0f,  1.0f},{1.0f, 1.0f}},
                {{-1.0f,  1.0f,  1.0f},{0.0f,  0.0f,  1.0f},{0.0f, 1.0f}},
                // Back face
                {{-1.0f, -1.0f, -1.0f},{0.0f,  0.0f, -1.0f},{1.0f, 0.0f}},
                {{ 1.0f, -1.0f, -1.0f},{0.0f,  0.0f, -1.0f},{0.0f, 0.0f}},
                {{ 1.0f,  1.0f, -1.0f},{0.0f,  0.0f, -1.0f},{0.0f, 1.0f}},
                {{-1.0f,  1.0f, -1.0f},{0.0f,  0.0f, -1.0f},{1.0f, 1.0f}},
                // Left face
                {{-1.0f, -1.0f, -1.0f},{-1.0f,  0.0f,  0.0f},{0.0f, 0.0f}},
                {{-1.0f, -1.0f,  1.0f},{-1.0f,  0.0f,  0.0f},{1.0f, 0.0f}},
                {{-1.0f,  1.0f,  1.0f},{-1.0f,  0.0f,  0.0f},{1.0f, 1.0f}},
                {{-1.0f,  1.0f, -1.0f},{-1.0f,  0.0f,  0.0f},{0.0f, 1.0f}},
                // Right face
                {{ 1.0f, -1.0f, -1.0f},{1.0f,  0.0f,  0.0f},{1.0f, 0.0f}},
                {{ 1.0f, -1.0f,  1.0f},{1.0f,  0.0f,  0.0f},{0.0f, 0.0f}},
                {{ 1.0f,  1.0f,  1.0f},{1.0f,  0.0f,  0.0f},{0.0f, 1.0f}},
                {{ 1.0f,  1.0f, -1.0f},{1.0f,  0.0f,  0.0f},{1.0f, 1.0f}},
                // Top face
                {{-1.0f,  1.0f, -1.0f},{0.0f,  1.0f,  0.0f},{0.0f, 1.0f}},
                {{-1.0f,  1.0f,  1.0f},{0.0f,  1.0f,  0.0f},{0.0f, 0.0f}},
                {{ 1.0f,  1.0f,  1.0f},{0.0f,  1.0f,  0.0f},{1.0f, 0.0f}},
                {{ 1.0f,  1.0f, -1.0f},{0.0f,  1.0f,  0.0f},{1.0f, 1.0f}},
                // Bottom face
                {{-1.0f, -1.0f, -1.0f},{0.0f, -1.0f,  0.0f},{1.0f, 1.0f}},
                {{-1.0f, -1.0f,  1.0f},{0.0f, -1.0f,  0.0f},{1.0f, 0.0f}},
                {{ 1.0f, -1.0f,  1.0f},{0.0f, -1.0f,  0.0f},{0.0f, 0.0f}},
                {{ 1.0f, -1.0f, -1.0f},{0.0f, -1.0f,  0.0f},{0.0f, 1.0f}}
            };
            return vertices;
        }
        static std::vector<unsigned int> CreateIndices()
        {
            std::vector<unsigned int> indices = {
                // Front face (CCW)
                2, 3, 0,  0, 1, 2,
                // Back face (CCW)
                6, 5, 4,  4, 7, 6,
                // Left face (CCW)
                10,11,8, 8, 9, 10,
                // Right face (CCW)
                14,13,12, 12,15,14,
                // Top face
                18,19,16, 16,17,18,
                // Bottom face
                20,23,22, 22,21,20
            };
            return indices;
        }
};

class ShpereMesh:public OpenGLMesh
{
    public:
        ShpereMesh(int stacks, int slices, float radius):OpenGLMesh(CreateVertices(stacks, slices, radius),
                                                              CreateIndices(stacks, slices)){}
    private:
        static std::vector<Vertex> CreateVertices(int stacks, int slices, float radius)
        {
            std::vector<Vertex> vertices;
            constexpr float PI = 3.14159265359f;

            for (int lat = 0; lat <= stacks; ++lat)
            {
                float theta = lat * PI / stacks; // 위도 각도
                float sinTheta = sin(theta);
                float cosTheta = cos(theta);

                for (int lon = 0; lon <= slices; ++lon)
                {
                    float phi = lon * 2.0f * PI / slices; // 경도 각도
                    float sinPhi = sin(phi);
                    float cosPhi = cos(phi);

                    // ---- 위치 ----
                    glm::vec3 position(
                        radius * sinTheta * cosPhi,
                        radius * cosTheta,
                        radius * sinTheta * sinPhi
                    );

                    // ---- 법선 ---- (정규화된 방향 벡터)
                    glm::vec3 normal = glm::normalize(glm::vec3(
                        sinTheta * cosPhi,
                        cosTheta,
                        sinTheta * sinPhi
                    ));

                    // ---- 텍스처 좌표 ----
                    float u = static_cast<float>(lon) / slices;
                    float v = static_cast<float>(lat) / stacks;

                    Vertex vertex;
                    vertex.position = position;
                    vertex.normal = normal;
                    vertex.texcoord = glm::vec2(u, v);

                    vertices.push_back(vertex);
                }
            }
            return vertices;
        }

        static std::vector<unsigned int> CreateIndices(int stacks, int slices)
        {
            std::vector<unsigned int> indices;

            for (int lat = 0; lat < stacks; ++lat)
            {
                for (int lon = 0; lon < slices; ++lon)
                {
                    int current = lat * (slices + 1) + lon;
                    int next = current + slices + 1;
                    indices.push_back(current);
                    indices.push_back(current + 1);
                    indices.push_back(next);
                    indices.push_back(current + 1);
                    indices.push_back(next + 1);
                    indices.push_back(next);
                }
            }
            return indices;
        }
};

class ConeMesh:public OpenGLMesh
{
    public:
        ConeMesh(int slices, float radius, float height):OpenGLMesh(CreateVertices(slices, radius, height),
                                                              CreateIndices(slices)){}
    private:
        static std::vector<Vertex> CreateVertices(int slices, float radius, float height)
        {
            std::vector<Vertex> vertices;
            constexpr float PI = 3.14159265359f;
            float halfHeight = height * 0.5f;
            glm::vec3 apex(0.0f, halfHeight, 0.0f);
            glm::vec3 baseCenter(0.0f, -halfHeight, 0.0f);
            for (int i = 0; i <= slices; ++i)
            {
                float theta = (2.0f * PI * i) / slices;
                float x = radius * cos(theta);
                float z = radius * sin(theta);
                glm::vec3 pos(x, -halfHeight, z);
                glm::vec3 normal = glm::normalize(glm::vec3(x, radius / height, z));
                glm::vec2 uv(static_cast<float>(i) / slices, 1.0f);

                vertices.push_back({pos, normal, uv});
            }
            vertices.push_back({apex, glm::vec3(0, 1, 0), glm::vec2(0.5f, 0)});
            vertices.push_back({baseCenter, glm::vec3(0, -1, 0), glm::vec2(0.5f, 0.5f)});

            return vertices;        
        }
        static std::vector<unsigned int> CreateIndices(int slices)
        {
            std::vector<unsigned int> indices;

            int apexIndex = slices + 1;
            int baseCenterIndex = apexIndex + 1;
            for (int i = 0; i < slices; ++i)
            {
                int next = (i + 1) % slices;
                indices.push_back(apexIndex);
                indices.push_back(i);
                indices.push_back(next);
            }
            for (int i = 0; i < slices; ++i)
            {
                int next = (i + 1) % slices;
                indices.push_back(baseCenterIndex);
                indices.push_back(next);
                indices.push_back(i);
            }

            return indices;
        }
};

class CylinderMesh:public OpenGLMesh
{
    public:
        CylinderMesh(int slices, float radius, float height):OpenGLMesh(CreateVertices(slices, radius, height),
                                                                  CreateIndices(slices)){}
    private:
        static std::vector<Vertex> CreateVertices(int slices, float radius, float height)
        {
            std::vector<Vertex> vertices;
            constexpr float PI = 3.14159265359f;
            float halfHeight = height * 0.5f;
            for (int i = 0; i <= slices; ++i)
            {
                float theta = (2.0f * PI * i) / slices;
                float x = radius * cos(theta);
                float z = radius * sin(theta);
                glm::vec3 normal = glm::normalize(glm::vec3(x, 0.0f, z));
                float u = static_cast<float>(i) / slices;
                vertices.push_back({ glm::vec3(x,  halfHeight, z), normal, glm::vec2(u, 0.0f) });
                vertices.push_back({ glm::vec3(x, -halfHeight, z), normal, glm::vec2(u, 1.0f) });
            }
            glm::vec3 topNormal = glm::vec3(0, 1, 0);
            for (int i = 0; i <= slices; ++i)
            {
                float theta = (2.0f * PI * i) / slices;
                float x = radius * cos(theta);
                float z = radius * sin(theta);
                float u = (cos(theta) + 1.0f) * 0.5f;
                float v = (sin(theta) + 1.0f) * 0.5f;

                vertices.push_back({ glm::vec3(x, halfHeight, z), topNormal, glm::vec2(u, v) });
            }
            vertices.push_back({ glm::vec3(0, halfHeight, 0), topNormal, glm::vec2(0.5f, 0.5f) });
            glm::vec3 bottomNormal = glm::vec3(0, -1, 0);
            for (int i = 0; i <= slices; ++i)
            {
                float theta = (2.0f * PI * i) / slices;
                float x = radius * cos(theta);
                float z = radius * sin(theta);
                float u = (cos(theta) + 1.0f) * 0.5f;
                float v = (sin(theta) + 1.0f) * 0.5f;

                vertices.push_back({ glm::vec3(x, -halfHeight, z), bottomNormal, glm::vec2(u, v) });
            }
            vertices.push_back({ glm::vec3(0, -halfHeight, 0), bottomNormal, glm::vec2(0.5f, 0.5f) });
            int bottomCenterIndex = static_cast<int>(vertices.size()) - 1;
            return vertices;
        }

        static std::vector<unsigned int> CreateIndices(int slices)
        {
            std::vector<unsigned int> indices;
            for (int i = 0; i < slices; ++i)
            {
                int top1 = i * 2;
                int bottom1 = top1 + 1;
                int top2 = ((i + 1) * 2);
                int bottom2 = top2 + 1;
                indices.push_back(top1);
                indices.push_back(top2);
                indices.push_back(bottom1);
                indices.push_back(top2);
                indices.push_back(bottom2);
                indices.push_back(bottom1);
            }
            int topStartIndex = (slices + 1) * 2;
            int topCenterIndex = topStartIndex + slices + 1;
            for (int i = 0; i < slices; ++i)
            {
                int curr = topStartIndex + i;
                int next = topStartIndex + (i + 1);
                indices.push_back(topCenterIndex);
                indices.push_back(next);
                indices.push_back(curr);
            }
            int bottomStartIndex = topCenterIndex + 1;
            int bottomCenterIndex = bottomStartIndex + slices + 1;

            for (int i = 0; i < slices; ++i)
            {
                int curr = bottomStartIndex + i;
                int next = bottomStartIndex + (i + 1);
                indices.push_back(bottomCenterIndex);
                indices.push_back(curr);
                indices.push_back(next);
            }
            return indices;
        }
};

class PlaneMesh:public OpenGLMesh
{
    public:
        PlaneMesh(float size = 1.0f):OpenGLMesh(CreatePlaneVertices(size),
                                          CreatePlaneIndices()){}
    private:
        static std::vector<Vertex> CreatePlaneVertices(float size)
        {
            float s = size * 0.5f;
            return {
                {{-s, 0, -s}, {0, 1, 0}, {0, 0}},
                {{ s, 0, -s}, {0, 1, 0}, {1, 0}},
                {{ s, 0,  s}, {0, 1, 0}, {1, 1}},
                {{-s, 0,  s}, {0, 1, 0}, {0, 1}},
            };
        }
        static std::vector<unsigned int> CreatePlaneIndices()
        {
            return { 0, 2, 1, 0, 3, 2 };
        }
};
