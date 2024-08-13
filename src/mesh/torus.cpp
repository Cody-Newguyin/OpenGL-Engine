#include "mesh/torus.h"

Torus::Torus(float radius1, float radius2, unsigned int numSteps1, unsigned int numSteps2) {
    vertices.resize((numSteps1 + 1) * (numSteps2 + 1));

    std::vector<glm::vec3> p(numSteps1 + 1);
    float a = 0.0f;
    float step = 2.0f * M_PI / numSteps1;
    for (int i = 0; i <= numSteps1; i++) {
        float x = cos(a) * radius1;
        float y = sin(a) * radius1;
        p[i].x = x;
        p[i].y = y;
        p[i].z = 0.0f;
        a += step;
    }

    for (int i = 0; i <= numSteps1; i++) {
        glm::vec3 u = glm::normalize(glm::vec3(0.0f) - p[i]) * radius2;     
        glm::vec3 v = glm::vec3(0.0f, 0.0f, 1.0f) * radius2;

        float a = 0.0f;
        float step = 2.0f * M_PI / numSteps2;
        for (int j = 0; j <= numSteps2; j++) {
            float c = cos(a);
            float s = sin(a);

            vertices[i * (numSteps2 + 1) + j].position = p[i] + c * u + s * v;
            vertices[i * (numSteps2 + 1) + j].uv.x = ((float)i) / ((float)numSteps1) * 2 * M_PI; 
            vertices[i * (numSteps2 + 1) + j].uv.y = ((float)j) / ((float)numSteps2);
            vertices[i * (numSteps2 + 1) + j].normal = glm::normalize(c * u + s * v);
            a += step;
        }
    }

    for (int i = 0; i < numSteps1; i++) {
        int i1 = i;
        int i2 = (i1 + 1);

        for (int j = 0; j < numSteps2; j++) {
            int j1 = j;
            int j2 = (j1 + 1);

            indices.push_back(i1 * (numSteps2 + 1) + j1);
            indices.push_back(i1 * (numSteps2 + 1) + j2);
            indices.push_back(i2 * (numSteps2 + 1) + j1);

            indices.push_back(i2 * (numSteps2 + 1) + j2);
            indices.push_back(i2 * (numSteps2 + 1) + j1);
            indices.push_back(i1 * (numSteps2 + 1) + j2);
        }
    }

    primitive = GL_TRIANGLES;
    LoadBuffers();
}