#ifndef _MESH_H_
#define _MESH_H_

#include <glad.h>
#include <glfw3.h>
#include <glm.hpp>
#include <vector>

struct VertexData {
    glm::vec3 position;
    glm::vec2 uv;
    glm::vec3 normal;
    glm::vec4 tangent;
};

class Mesh {
    public:
    // ID for the VAO buffer
    unsigned int VAO;
    
    // Mesh information
    std::vector<VertexData> vertices;
    std::vector<unsigned int> indices;
    
    bool EBOenabled; // Draw with indices if true
    unsigned int primitive = GL_TRIANGLES; // Defines the primitive drawn
    bool Log = false;
    
    public:
    Mesh();
    void LoadBuffers();
    void Unload();

    private:
    unsigned int VBO, EBO;

    private:
    void CalculateTangents();
    virtual void LoadData();
    virtual void LinkAttributes();


};

#endif