#ifndef _SCENE_OBJECT_H_
#define _SCENE_OBJECT_H_

#include <string>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "mesh/mesh.h"
#include "shader/material.h"

class SceneObject {
    public:
    // Object information
    std::string name = "text";

    // Render information
    Mesh *mesh = NULL;
    Material *material = NULL;
    bool shadowCast = true;
    
    // Transform information
    glm::mat4 transform = glm::mat4(1.0f);
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    // Position after applying transform
    glm::vec3 worldPos;

    // Scene relationship
    std::vector<SceneObject*> children;
    SceneObject* parent = NULL;
    
    public:
    SceneObject(std::string name = "text");
    SceneObject(Mesh *mesh, Material *material, std::string name = "text");
    ~SceneObject();

    void AddObject(SceneObject *child);

    void SetName(std::string name);
    void SetMesh(Mesh *mesh);
    void SetMaterial(Material* material);
    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetScale(glm::vec3 scale);

    // Main transform update
    void UpdateTransform();
    // Alternate transform update that uses glm transform functions
    void UpdateTransformAlt();
    
    protected:
    bool dirty = false;
};

#endif