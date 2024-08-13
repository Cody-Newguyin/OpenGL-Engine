#ifndef _OBJECT_LOADER_H_
#define _OBJECT_LOADER_H_

#include <vector>
#include <string>
#include <scene/scene_object.h>
#include <mesh/mesh.h>
#include <shader/basic_material.h>


class ObjectLoader {
    public:
    // The default material for loaded objects when not specified
    BasicMaterial* defaultMat;
    // All new materials created for objects are stored here and cleared after calling ReadObjFile
    std::vector<BasicMaterial*> storedMats;

    public:
    ObjectLoader();
    SceneObject* ReadObjFile(std::string filename, bool useDefault = true, bool flipImage = false, NORM_MAP_TYPE type = NORM_MAP_NONE);
};

#endif