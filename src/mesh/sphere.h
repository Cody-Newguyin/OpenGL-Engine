#ifndef _MESH_SPHERE_H_
#define _MESH_SPHERE_H_

#include "mesh/mesh.h"

class Sphere : public Mesh {
    public:
    Sphere(unsigned int xSegments, unsigned int ySegments);
};

#endif