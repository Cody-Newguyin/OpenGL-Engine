#ifndef _SCENE_BACKGROUND_H_
#define _SCENE_BACKGROUND_H_

#include "scene/scene_object.h"
#include "shader/texture_cube.h"

class Background : public SceneObject {
    public:
    TextureCube* envMap;
    
    public:
    Background();
    void SetEnvMap(TextureCube* envMap);
};

#endif