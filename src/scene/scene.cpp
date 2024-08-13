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

void Scene::Update() {
    for (unsigned int i = 0; i < root->children.size(); i++) {
        root->children[i]->UpdateTransform();
    }
    for (unsigned int i = 0; i < dirLights.size(); i++) {
        dirLights[i]->UpdateTransform();
    }
    for (unsigned int i = 0; i < pointLights.size(); i++) {
        pointLights[i]->UpdateTransform();
    }
}

void Scene::Clear() {
    root->children.clear();
    pointLights.clear();
    dirLights.clear();
}
