#ifndef _SHADER_H_
#define _SHADER_H_

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <gtc/type_ptr.hpp>
#include <string>
#include <vector>

class Shader {
    public:
    unsigned int programID, vertexShaderID, fragmentShaderID, geometryShaderID;
    std::string vertexFilename;
    std::string fragmentFilename;
    std::vector<std::string> defines;

    public:
    Shader();
    void Initialize(std::string vertexFilename, std::string fragmentFilename, std::string geometryFilename = "");
    void Use();
    void Unload();
    void AddDefine(std::string define);

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