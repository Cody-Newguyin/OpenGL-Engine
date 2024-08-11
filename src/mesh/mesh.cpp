#include "mesh/mesh.h"
#include "mesh.h"
#include "log/log.h"

Mesh::Mesh() {

}

void Mesh::LoadBuffers() {
    EBOenabled = !indices.empty();

    // Generate an ids for buffers/arrays
    glGenVertexArrays(1, &(VAO));
    glGenBuffers(1, &(VBO));  
    // Bind VAO buffer
    glBindVertexArray(VAO);
    // Bind VBO buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);  

    if (EBOenabled) {
        // Generate an ids for buffer
        glGenBuffers(1, &(EBO));
        // Bind EBO buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        // Copy index data to buffer
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    CalculateTangents();
    LoadData();
    LinkAttributes();

    // unbind buffers
    // VBO has to be registerd for vertex attributes before unbinding
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    glBindVertexArray(0); 
    if (EBOenabled) {
        // VAO has to be unbound before EBO as it is stored in the VAO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
}

void Mesh::CalculateTangents() {
    if (Log) LOG_INFO("START");
    if (Log) LOG_ERROR(vertices.size());
    if (Log) LOG_ERROR(indices.size());
    for (unsigned int i = 0; i < vertices.size(); i++) vertices[i].tangent = glm::vec4(0.0f);
    for (unsigned int i = 0; i < indices.size(); i++) {
        
        int j = indices[i];
        int k = indices[(i + 1) % 3 + i / 3 * 3];
        int l = indices[(i + 2) % 3 + i / 3 * 3];
        if (Log) LOG_ERROR("Vertex: " + std::to_string(j));
        if (vertices[j].position == vertices[k].position || vertices[k].position == vertices[l].position || vertices[l].position == vertices[j].position) continue;
        glm::vec3 deltaPos1 = vertices[k].position - vertices[j].position;
        glm::vec3 deltaPos2 = vertices[l].position - vertices[j].position;
        if (deltaPos1 == deltaPos2) continue;
        glm::vec2 deltaUV1 = vertices[k].uv - vertices[j].uv;
        glm::vec2 deltaUV2 = vertices[l].uv - vertices[j].uv;
        glm::vec3 normal = vertices[j].normal;

        float flip = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x) > 0 ? 1 : -1;

        if (Log) LOG_GLM(deltaPos1);
        if (Log) LOG_GLM(deltaPos2);

        deltaPos1 -= normal * glm::dot(deltaPos1, normal);
        deltaPos2 -= normal * glm::dot(deltaPos2, normal);
       
        if (Log) LOG_GLM(deltaUV1);
        if (Log) LOG_GLM(deltaUV2);
        
        float angle = std::acos(dot(deltaPos1, deltaPos2) / (length(deltaPos1) * length(deltaPos2)));
        if (Log) LOG_INFO((length(deltaPos1) * length(deltaPos2)));
        if (Log) LOG_INFO(angle);
        glm::vec3 tangent = normalize((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * flip);
        if (Log) LOG_GLM((deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y));
        if (Log) LOG_GLM(tangent);
        vertices[j].tangent.w = -flip;
        vertices[j].tangent += glm::vec4((tangent * angle), 0);
    }
    for (unsigned int i = 0; i < vertices.size(); i++) {
        if (glm::vec3(vertices[i].tangent) != glm::vec3(0,0,0)) {
            vertices[i].tangent = glm::vec4(glm::normalize(glm::vec3(vertices[i].tangent)), vertices[i].tangent.w);
        }
        if (Log) LOG_GLM(vertices[i].tangent);
    }
    if (Log) LOG_INFO("END");
}

void Mesh::LoadData() {
    // Copy vertex data to buffer
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexData), &vertices[0], GL_STATIC_DRAW);
}

void Mesh::LinkAttributes() {
    // linking vertex shader attributes, defines how opengl reads in input for vertex shader
    // useses offsetof (perprocesser macro to calculate byte offset)
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, position));
    glEnableVertexAttribArray(0);
    // texture coords attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, uv));
    glEnableVertexAttribArray(1); 
    // normal attribute
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, normal));
    glEnableVertexAttribArray(2); 
    // tangent attribute
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tangent));
    glEnableVertexAttribArray(3); 
}

void Mesh::Unload()
{
    glDeleteVertexArrays(1, &(VAO));
    glDeleteBuffers(1, &(VBO));
    if (EBOenabled) {
        glDeleteBuffers(1, &EBO);
    }
}
