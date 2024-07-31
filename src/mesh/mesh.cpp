#include "mesh/mesh.h"

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
}

void Mesh::Unload()
{
    glDeleteVertexArrays(1, &(VAO));
    glDeleteBuffers(1, &(VBO));
    if (EBOenabled) {
        glDeleteBuffers(1, &EBO);
    }
}
