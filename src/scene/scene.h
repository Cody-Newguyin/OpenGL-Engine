#ifndef _SCENE_H_
#define _SCENE_H_

#include <vector>

#include "scene/light/light_object.h"
#include "scene/scene_object.h"
#include "scene/background.h"

class Scene {
    public:
    static SceneObject* root;
    Background* background;
    std::vector<LightObject*> pointLights;
    std::vector<LightObject*> dirLights;

    public:
    Scene();
    void AddObject(SceneObject*object);
    void AddLight(LightObject* light);
    void Clear();

    private:
};

#endif