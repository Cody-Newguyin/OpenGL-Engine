#include "utility/utility.h"
#include "log/log.h"
#include <fstream>
#include <sstream>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

bool ReadFile(std::string filename, std::string& content, std::string type) {
    std::ifstream file;

    // Ensure ifstream objects can throw exceptions:
    file.exceptions (std::ifstream::failbit | std::ifstream::badbit);

    try {
        // Open files
        file.open(filename);
        std::stringstream fileStream;
        // Read file's buffer contents into streams
        fileStream << file.rdbuf();	
        // Close file handlers
        file.close();
        // Convert stream into string
        content = fileStream.str();		
    }
    catch(std::ifstream::failure e) {
        LOG_ERROR("ERROR::" + type + "(" + filename + ")FILE_NOT_SUCCESFULLY_READ");
        return false;
    }

    return true;
}

std::string ReadShader(std::string filename, std::string type) {
    std::ifstream file;
    std::string directory = filename.substr(0, filename.find_last_of("/\\"));
    std::string source, line;

    try {
        // Open file
        file.open(filename);
        while(std::getline(file, line)) {
            // attach another file string on #include
            if (line.substr(0, 8) == "#include") {
                // append include string to directory
                std::string includePath = directory + "/" + line.substr(9);
                source += ReadShader(includePath, type);
            } else {
                source += line + "\n";
            }
        }
        // Close file handlers
        file.close();
    }
    catch(std::ifstream::failure e) {
        LOG_ERROR("ERROR::" + type + "(" + filename + ")FILE_NOT_SUCCESFULLY_READ");
        return "";
    }
    
    return source;
}

std::vector<Mesh> ReadObjFile(std::string inputfile) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "";
    
    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(inputfile, reader_config)) {
        if(!reader.Error().empty()) {
            LOG_ERROR("TINYOBJ: " + reader.Error());
        }
    }
    if (!reader.Warning().empty()) {
        LOG_ERROR("TINYOBJ: " + reader.Warning());
    }

    auto& attrib = reader.GetAttrib();
    auto& shapes = reader.GetShapes();
    auto& materials = reader.GetMaterials();

    std::vector<Mesh> meshes;
    Mesh mesh;
    VertexData vertex;
    for (size_t s = 0; s < shapes.size(); s++) {
        // Loop over faces(polygon)
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
                mesh.vertices.push_back(vertex);
            }
            index_offset += fv;
        }
        mesh.LoadBuffers();
        meshes.push_back(mesh);
    }

    return meshes;
}
