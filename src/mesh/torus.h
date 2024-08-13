#ifndef _MESH_TORUS_H_
#define _MESH_TORUS_H_

#include "mesh/mesh.h"

class Torus : public Mesh {
    public:
    Torus(float radius1, float radius2, unsigned int numSteps1, unsigned int numSteps2);
};

#endif