#ifndef _MESH_PLANE_H_
#define _MESH_PLANE_H_

#include "mesh/mesh.h"

class Plane : public Mesh {
    public:
    Plane(unsigned int xSegments, unsigned int ySegments);
};

#endif