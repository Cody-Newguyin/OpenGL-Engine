#include "material.h"

Material::Material() {

}

int Material::BindUniforms() {
    // Unbind textures
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    // Bind textures
    for (unsigned int i = 0; i < uniformSamplers.size(); i++) {
        // Bind texture unit with our loaded texture
        switch (uniformSamplers[i].type) {
        case SHADER_TYPE_SAMPLER2D:
            uniformSamplers[i].texture->Bind(uniformSamplers[i].unit);
            break;
        case SHADER_TYPE_SAMPLERCUBE:
            uniformSamplers[i].textureCube->Bind(uniformSamplers[i].unit);
            break;
        default:
            break;
        }
    }

    // Set uniform state
    for (unsigned int i = 0; i < uniforms.size(); i++) {
        switch (uniforms[i].type) {
            case SHADER_TYPE_BOOL:
                shader->SetBool(uniforms[i].name, *uniforms[i].Bool);
                break;
            case SHADER_TYPE_INT:
                shader->SetInt(uniforms[i].name, *uniforms[i].Int);
                break;
            case SHADER_TYPE_FLOAT:
                shader->SetFloat(uniforms[i].name, *uniforms[i].Float);
                break;
            case SHADER_TYPE_VEC2:
                shader->SetVector(uniforms[i].name, *uniforms[i].Vec2);
                break;
            case SHADER_TYPE_VEC3:
                shader->SetVector(uniforms[i].name, *uniforms[i].Vec3);
                break;
            case SHADER_TYPE_VEC4:
                shader->SetVector(uniforms[i].name, *uniforms[i].Vec4);
                break;
            case SHADER_TYPE_MAT2:
                shader->SetMatrix(uniforms[i].name, *uniforms[i].Mat2);
                break;
            case SHADER_TYPE_MAT3:
                shader->SetMatrix(uniforms[i].name, *uniforms[i].Mat3);
                break;
            case SHADER_TYPE_MAT4:
                shader->SetMatrix(uniforms[i].name, *uniforms[i].Mat4);
                break;
            default:
                break;
        }
    }

    return uniformSamplers.size();
}

void Material::SetShader(Shader* shader) {
    this->shader = shader;
    
    if (shader) {
        shader->Use();
        for (unsigned int i = 0; i < uniformSamplers.size(); i++) {
            // Set uniform state of textures
            shader->SetInt(uniformSamplers[i].name, uniformSamplers[i].unit); 
        }
    }  
}

void Material::SetTexture(std::string name, Texture *texture) {
    UniformValueSampler uniformSampler;
    uniformSampler.name = name;
    uniformSampler.type = SHADER_TYPE_SAMPLER2D;
    uniformSampler.unit = uniformSamplers.size();
    uniformSampler.texture = texture;
    uniformSamplers.push_back(uniformSampler);

    if (shader) {
        shader->Use();
        shader->SetInt(uniformSampler.name, uniformSampler.unit);
    }
}

void Material::SetTextureCube(std::string name, TextureCube *textureCube) {
    UniformValueSampler uniformSampler;
    uniformSampler.name = name;
    uniformSampler.type = SHADER_TYPE_SAMPLERCUBE;
    uniformSampler.unit = uniformSamplers.size();
    uniformSampler.textureCube = textureCube;
    uniformSamplers.push_back(uniformSampler);
    
    if (shader) {
        shader->Use();
        shader->SetInt(uniformSampler.name, uniformSampler.unit);
    }
}

void Material::SetInt(std::string name, int *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_INT;
    uniform.Int = value;
    uniforms.push_back(uniform);
}

void Material::SetBool(std::string name, bool *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_BOOL;
    uniform.Bool = value;
    uniforms.push_back(uniform);
}

void Material::SetFloat(std::string name, float *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_FLOAT;
    uniform.Float = value;
    uniforms.push_back(uniform);
}

void Material::SetVector(std::string name, glm::vec2 *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_VEC2;
    uniform.Vec2 = value;
    uniforms.push_back(uniform);
}

void Material::SetVector(std::string name, glm::vec3 *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_VEC3;
    uniform.Vec3 = value;
    uniforms.push_back(uniform);
}

void Material::SetVector(std::string name, glm::vec4 *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_VEC4;
    uniform.Vec4 = value;
    uniforms.push_back(uniform);
}

void Material::SetMatrix(std::string name, glm::mat2 *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_MAT2;
    uniform.Mat2 = value;
    uniforms.push_back(uniform);
}

void Material::SetMatrix(std::string name, glm::mat3 *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_MAT3;
    uniform.Mat3 = value;
    uniforms.push_back(uniform);
}

void Material::SetMatrix(std::string name, glm::mat4 *value) {
    UniformValue uniform;
    uniform.name = name;
    uniform.type = SHADER_TYPE_MAT4;
    uniform.Mat4 = value;
    uniforms.push_back(uniform);
}

