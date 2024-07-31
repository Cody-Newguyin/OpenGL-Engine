#include "utility/utility.h"
#include "log/log.h"
#include <fstream>
#include <sstream>

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