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
    NORM_MAP_TYPE normType = NORM_MAP_NONE;
    SMOOTH_MAP_TYPE smoothType = SMOOTH_MAP_NONE;

    public:
    ObjectLoader();
    SceneObject* ReadObjFile(std::string filename, bool useDefault = true, bool flipImage = false);
};

#endif