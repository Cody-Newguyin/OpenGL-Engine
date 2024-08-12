#ifndef _SHADER_MATERIAL_H_
#define _SHADER_MATERIAL_H_

#include <glad.h>
#include <glfw3.h>
#include <string>
#include <vector>

#include "shader/shader.h"
#include "shader/texture.h"
#include "shader/texture_cube.h"

enum SHADER_TYPE {
    SHADER_TYPE_BOOL,
    SHADER_TYPE_INT,
    SHADER_TYPE_FLOAT,
    SHADER_TYPE_SAMPLER2D,
    SHADER_TYPE_SAMPLERCUBE,
    SHADER_TYPE_VEC2,
    SHADER_TYPE_VEC3,
    SHADER_TYPE_VEC4,
    SHADER_TYPE_MAT2,
    SHADER_TYPE_MAT3,
    SHADER_TYPE_MAT4,
};

struct UniformValueSampler {
    std::string name;
    SHADER_TYPE type;
    unsigned int unit;
    union {
        Texture* texture;
        TextureCube* textureCube;
    };
    
};

struct UniformValue {
    std::string name;
    SHADER_TYPE type;
    union {
        bool      *Bool;
        int       *Int;
        float     *Float;
        glm::vec2 *Vec2;
        glm::vec3 *Vec3;
        glm::vec4 *Vec4;
        glm::mat2 *Mat2;
        glm::mat3 *Mat3;
        glm::mat4 *Mat4;
    };
};

class Material {
    public:
    std::string name = "text";
    Shader* shader = NULL;
    std::vector<UniformValueSampler> uniformSamplers;
    std::vector<UniformValue> uniforms;

    // depth state
    GLenum DepthCompare = GL_LESS;
    
    public:
    Material(std::string name = "text");
    int BindUniforms();
    void SetShader(Shader* shader);
    void SetTexture(std::string name, Texture *texture);
    void SetTextureCube(std::string name, TextureCube *textureCube);
    void SetInt(std::string name, int *value);
    void SetBool(std::string name, bool *value);
    void SetFloat(std::string name, float *value);
    void SetVector(std::string name, glm::vec2 *value);
    void SetVector(std::string name, glm::vec3 *value);
    void SetVector(std::string name, glm::vec4 *value);
    void SetMatrix(std::string name, glm::mat2 *value);
    void SetMatrix(std::string name, glm::mat3 *value);
    void SetMatrix(std::string name, glm::mat4 *value);
};

#endif