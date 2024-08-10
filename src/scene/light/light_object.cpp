#include "light_object.h"
#include "log/log.h"

LightObject::LightObject(LIGHT_TYPE type) {
    this->type = type;
    this->shadowCast = false;
    this->dirty = true;
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

// this is wack
glm::vec3 *LightObject::GetRealColor() {
    this->realColor = this->color * this->intensity;
    return &this->realColor;
}
