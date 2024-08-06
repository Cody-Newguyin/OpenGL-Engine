#include "sphere.h"

Sphere::Sphere(unsigned int xSegments, unsigned int ySegments) {
    for (unsigned int y = 0; y <= ySegments; y++) {
        for (unsigned int x = 0; x <= xSegments; x++) {
            float xSegment = (float)x / (float)xSegments;
            float ySegment = (float)y / (float)ySegments;
            float xPos = std::cos(xSegment * 2 * M_PI) * std::sin(ySegment * M_PI); // TAU is 2PI
            float yPos = std::cos(ySegment * M_PI);
            float zPos = std::sin(xSegment * 2 * M_PI) * std::sin(ySegment * M_PI);

            VertexData vertex;
            vertex.position = glm::vec3(xPos, yPos, zPos) / 2.0f;
            vertex.uv = glm::vec2(xSegment, ySegment);
            vertex.normal = glm::vec3(xPos, yPos, zPos);
            vertices.push_back(vertex);
        }
    }

    for (int y = 0; y < ySegments; y++) {
        for (int x = 0; x < xSegments; x++) {
            indices.push_back((y + 1) * (xSegments + 1) + x);
            indices.push_back(y       * (xSegments + 1) + x);
            indices.push_back(y       * (xSegments + 1) + x + 1);

            indices.push_back((y + 1) * (xSegments + 1) + x);
            indices.push_back(y       * (xSegments + 1) + x + 1);
            indices.push_back((y + 1) * (xSegments + 1) + x + 1);
        }
    }

    primitive = GL_TRIANGLE_STRIP;
    LoadBuffers();
}