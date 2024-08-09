#ifndef _OBJECT_LOADER_H_
#define _OBJECT_LOADER_H_

#include <vector>
#include <string>
#include <scene/scene_object.h>
#include <mesh/mesh.h>
#include <shader/basic_material.h>


class ObjectLoader {
    public:
    static SceneObject* ReadObjFile(std::string inputfile);
};

#endif