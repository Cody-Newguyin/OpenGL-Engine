#include "scene_object.h"

SceneObject::SceneObject() {
    
}

SceneObject::SceneObject(Mesh *mesh, Material *material) {
    this->mesh = mesh;
    this->material = material;
}

SceneObject::~SceneObject() {
    children.clear();
    if (mesh) {
        mesh->Unload();
    }
}

void SceneObject::AddObject(SceneObject* child) {
    child->parent = this;
    children.push_back(child);

    // Ensure that child updates its transform
    child->dirty = true;
}

void SceneObject::SetName(std::string name) {
    this->name = name;
}

void SceneObject::SetMesh(Mesh *mesh) {
    this->mesh = mesh;
}

void SceneObject::SetMaterial(Material* material) {
    this->material = material;
}

void SceneObject::SetPosition(glm::vec3 position) {
    this->position = position;
    dirty = true;
}

void SceneObject::SetScale(glm::vec3 scale) {
    this->scale = scale;
    dirty = true;
}

void SceneObject::SetRotation(glm::vec3 rotation) {
    this->rotation = rotation;
    dirty = true;
}

void SceneObject::UpdateTransformAlt() {
    // I don't know how to rotate along 1 axis for 3 x,y,z angles
    transform = glm::mat4(1.0f);
    transform = glm::scale(transform, scale);
    transform = glm::rotate(transform, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    transform = glm::rotate(transform, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
    transform = glm::translate(transform, position);
}

void SceneObject::UpdateTransform() {
    // Only update transform if a change was made
    if (!dirty) {
        return;
    }

    // Translate
    glm::mat4 translateMatrix = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        position.x, position.y, position.z, 1.0f
    );

    // Scale
    glm::mat4 scaleMatrix = glm::mat4(
        scale.x, 0.0f, 0.0f, 0.0f,
        0.0f, scale.y, 0.0f, 0.0f,
        0.0f, 0.0f, scale.z, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );

    // Rotate
    float radX = glm::radians(rotation.x);
    float radY = glm::radians(rotation.y);
    float radZ = glm::radians(rotation.z);
    float sinX = sin(radX);
    float cosX = cos(radX);
    float sinY = sin(radY);
    float cosY = cos(radY);
    float sinZ = sin(radZ);
    float cosZ = cos(radZ);
    glm::mat4 rotateMatrix = glm::mat4(
        cosY * cosZ,  cosX * sinZ + sinX * sinY * cosZ, sinX * sinZ - cosX * sinY * cosZ, 0.0f,
        -cosY * sinZ, cosX * cosZ - sinX * sinY * sinZ, sinX * cosZ + cosX * sinY * sinZ, 0.0f,
        sinY,        -sinX * cosY,                      cosX * cosY,                      0.0f,
        0.0f,         0.0f,                             0.0f,                             1.0f
    );

    // Note: Why is this in reverse order!?
    // transform = glm::mat4(1.0f);  
    transform = translateMatrix;
    transform = transform * rotateMatrix;
    transform = transform * scaleMatrix;

    // Get transform relative to parent by multiplying matrices
    if (parent) {
        transform = parent->transform * transform; 
    }

    // Update children matrices
    for (int i = 0; i < children.size(); ++i) {
        children[i]->dirty = true;
        children[i]->UpdateTransform();
    }

    worldPos = transform * glm::vec4(position, 1.0f);

    dirty = false;
}

