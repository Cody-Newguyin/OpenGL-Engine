#ifndef _LIGHT_LIGHTOBJECT_H_
#define _LIGHT_LIGHTOBJECT_H_

#include <glm.hpp>

#include "scene/scene_object.h"

enum LIGHT_TYPE {
    LIGHT_TYPE_DIR,
    LIGHT_TYPE_POINT,
};

class LightObject : public SceneObject {
    public:
    glm::vec3 color = glm::vec3(1.0f);
    glm::vec3 direction;
    float intensity = 1.0f;
    LIGHT_TYPE type;

    glm::mat4 spaceMatrix;
    
    public:
    LightObject(LIGHT_TYPE type);
    void UpdateTransform();
};

#endif