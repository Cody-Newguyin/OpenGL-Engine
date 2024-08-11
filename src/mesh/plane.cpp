#include "mesh/plane.h"

Plane::Plane(unsigned int xSegments, unsigned int ySegments) {
    this->Log = true;
    
    float dx = 2.0f / xSegments;
    float dy = 2.0f / ySegments;

    for (int y = 0; y <= ySegments; y++) {
        for (int x = 0; x <= xSegments; x++) {
            VertexData vertex;
            vertex.position = glm::vec3(x * dx - 1.0f, y * dy - 1.0f, 0.0f);
            vertex.uv = glm::vec2(dx * x, 1.0f - y * dy);
            vertex.normal = glm::vec3(0.0f, 0.0f, 1.0f);
            vertices.push_back(vertex);
        }
    }

    bool oddRow = false;
    for (int y = 0; y < ySegments; y++) {
        if (!oddRow) {// even rows: y == 0, y == 2; and so on 
            for (int x = 0; x <= xSegments; x++) {
                indices.push_back(y       * (xSegments + 1) + x);
                indices.push_back((y + 1) * (xSegments + 1) + x);
            }
        }
        else {
            for (int x = xSegments; x >= 0; x--) {
                indices.push_back((y + 1) * (xSegments + 1) + x);
                indices.push_back(y       * (xSegments + 1) + x);
            }
        }
        oddRow = !oddRow;
    }

    primitive = GL_TRIANGLE_STRIP;
    LoadBuffers();
}