#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <string>
#include "mesh/mesh.h"

bool ReadFile(std::string filename, std::string& content, std::string type);
std::string ReadShader(std::string filename, std::string type);
std::vector<Mesh> ReadObjFile(std::string inputfile);

#endif