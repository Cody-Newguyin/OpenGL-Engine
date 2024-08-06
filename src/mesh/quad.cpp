#include "quad.h"

Quad::Quad() {
    std::vector<glm::vec3> positions = {
        glm::vec3(-1.0f,  1.0f, 0.0f),
        glm::vec3(-1.0f, -1.0f, 0.0f),
        glm::vec3(1.0f,  1.0f, 0.0f),
        glm::vec3(1.0f, -1.0f, 0.0f)
    };
    std::vector<glm::vec2> uv = {
        glm::vec2(0.0f, 1.0f),
        glm::vec2(0.0f, 0.0f),
        glm::vec2(1.0f, 1.0f),
        glm::vec2(1.0f, 0.0f),
    };
    

    for (int i = 0; i < positions.size(); i++) {
        VertexData vertex;
        vertex.position = positions[i];
        vertex.uv = uv[i];
        vertex.normal = glm::vec3(0.0f);
        vertices.push_back(vertex);
    }

    primitive = GL_TRIANGLE_STRIP;
    LoadBuffers();
}
