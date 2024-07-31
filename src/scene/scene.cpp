#include "scene.h"

SceneObject* Scene::root = new SceneObject();

Scene::Scene() {

}

void Scene::AddObject(SceneObject *object) {
    root->AddObject(object);
}

void Scene::AddLight(LightObject *light) {
    if (light->type == LIGHT_TYPE_POINT) {
        pointLights.push_back(light);
    } else if (light->type == LIGHT_TYPE_DIR) {
        dirLights.push_back(light);
    }
    
}

void Scene::Clear() {
    root->children.clear();
    pointLights.clear();
    dirLights.clear();
}
