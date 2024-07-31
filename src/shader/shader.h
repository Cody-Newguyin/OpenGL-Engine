#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <string>

class Shader {
    public:
    unsigned int programID, vertexShaderID, fragmentShaderID;
    std::string vertexFilename;
    std::string fragmentFilename;

    public:
    Shader();
    void Initialize(std::string vertexFilename, std::string fragmentFilename);
    void Use();
    void Unload();

    void SetInt(std::string location, int value);
    void SetBool(std::string location, bool value);
    void SetFloat(std::string location, float value);
    void SetVector(std::string location, glm::vec2 value);
    void SetVector(std::string location, glm::vec3 value);
    void SetVector(std::string location, glm::vec4 value);
    void SetMatrix(std::string location, glm::mat2 value);
    void SetMatrix(std::string location, glm::mat3 value);
    void SetMatrix(std::string location, glm::mat4 value);
};

#endif