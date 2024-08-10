#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <string>

bool ReadFile(std::string filename, std::string& content, std::string type);
std::string ReadShader(std::string filename, std::string type, std::string defines);

#endif