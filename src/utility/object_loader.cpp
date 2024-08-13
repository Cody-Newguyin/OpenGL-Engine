#include "object_loader.h"
#include "log/log.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

ObjectLoader::ObjectLoader() {
    defaultMat = new BasicMaterial("Default");
    defaultMat->Initalize();
}

SceneObject* ObjectLoader::ReadObjFile(std::string filename, bool useDefault, bool flipImage, NORM_MAP_TYPE type) {
    tinyobj::ObjReader reader;
    tinyobj::ObjReaderConfig reader_config;
    std::string directory = filename.substr(0, filename.find_last_of("/\\")) + "/";
    reader_config.mtl_search_path = "";

    if (!reader.ParseFromFile(filename, reader_config)) {
        if(!reader.Error().empty()) {
            LOG_ERROR("TINYOBJ: " + reader.Error());
            return NULL;
        }
    }
    if (!reader.Warning().empty()) {
        // LOG_ERROR("TINYOBJ: " + reader.Warning());
    }
    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    // Create list of materials
    storedMats.clear();
    if (!useDefault) {
        for (size_t m = 0; m < materials.size(); m++) {
            BasicMaterial* basicMat = new BasicMaterial(materials[m].name);
            basicMat->SetFlipImage(flipImage);
            if (!materials[m].diffuse_texname.empty()) {
                basicMat->SetMainFile(directory + materials[m].diffuse_texname);
            }
            if (!materials[m].bump_texname.empty()) {
                basicMat->SetNormalFile(directory + materials[m].bump_texname, type);
            }
            basicMat->Initalize();
            storedMats.push_back(basicMat);
        }   
    }
    
    LOG_INFO("Number of child objects: " + std::to_string(shapes.size()));
    SceneObject* object = new SceneObject();
    for (size_t s = 0; s < shapes.size(); s++) {
        Mesh* mesh = new Mesh();
        VertexData vertex;
        Material* material;

        int currentMatID = shapes[s].mesh.material_ids[0];
        // Loop over faces(polygon) and build mesh
        size_t index_offset = 0;
        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

            // Loop over vertices in the face.
            for (size_t v = 0; v < fv; v++) {
                // access to vertex
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
                tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
                tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];
                vertex.position = glm::vec3(vx, vy, vz);

                // Check if `normal_index` is zero or positive. negative = no normal data
                if (idx.normal_index >= 0) {
                    tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
                    tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
                    tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];
                    vertex.normal = glm::vec3(nx, ny, nz);
                }

                // Check if `texcoord_index` is zero or positive. negative = no texcoord data
                if (idx.texcoord_index >= 0) {
                    tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
                    tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
                    vertex.uv = glm::vec2(tx, ty);
                }

                // Optional: vertex colors
                // tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
                // tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
                // tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
                mesh->vertices.push_back(vertex);
            }
            index_offset += fv;

            // Create a child object when it has a new material or passes all vertices
            if (currentMatID != shapes[s].mesh.material_ids[f] || f == (shapes[s].mesh.num_face_vertices.size() - 1)) {
                // Set material to default if unspecified or face's material
                if (currentMatID == -1 || useDefault)  {
                    material = defaultMat;
                } else {
                    material = storedMats[currentMatID];
                }
                
                // Finalize and add as child object;
                mesh->LoadBuffers();
                SceneObject* childObject = new SceneObject(mesh, material);
                object->AddObject(childObject);
                // Set new currentMatID
                currentMatID = shapes[s].mesh.material_ids[f];
                mesh = new Mesh();
            }
        }
    }
    return object;
}
