#include "light_object.h"
#include "log/log.h"

LightObject::LightObject(LIGHT_TYPE type) {
    this->type = type;
    this->shadowCast = false;
    UpdateTransform();
}

void LightObject::UpdateTransform() {
    SceneObject::UpdateTransform();
    float radX = glm::radians(rotation.x);
    float radY = glm::radians(rotation.y);
    direction.x = cos(radY)*cos(radX);
    direction.y = sin(radY)*cos(radX);
    direction.z = sin(radX);
}
