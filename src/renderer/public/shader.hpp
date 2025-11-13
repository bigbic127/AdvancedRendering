#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

class IShader
{
    public:
        ~IShader() = default;
        virtual void UseProgram() = 0;
        virtual void EndProgam() = 0;
};

class OpenGLShader:public IShader
{
    public:
        OpenGLShader(const std::string& vsPath, const std::string& fsPath);
        ~OpenGLShader();
        void UseProgram() override;
        void EndProgam() override;
        void SetBool(const std::string& name, bool value) const;
        void SetInt(const std::string& name, int value) const;
        void SetFloat(const std::string& name, float value) const;
        void SetDouble(const std::string& name, int value) const;
        void SetVector3(const std::string& name, const glm::vec3& value) const;
        void SetMatrix4(const std::string& name, const glm::mat4& value) const;
        GLint GetLocation(const std::string& name) const;
    private:
        std::string LoadShaderSource(const std::string& path);
        unsigned int programID;
};