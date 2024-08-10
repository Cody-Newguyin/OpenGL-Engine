#include "shader/shader.h"
#include "utility/utility.h"
#include "log/log.h"
#include "shader.h"

Shader::Shader() {

}

void Shader::Initialize(std::string vertexFilename, std::string fragmentFilename, std::string geometryFilename) {
    this->vertexFilename = vertexFilename;
    this->fragmentFilename = fragmentFilename;

    // Setup defines
    std::string defineCode = "";
    for (unsigned int i = 0; i < defines.size(); i++) {
        defineCode +=  "#define " + defines[i] + " \n";
    }

    // Read in shader code
    std::string vertexCode = ReadShader(vertexFilename, "SHADER::Vertex", defineCode);
    std::string fragmentCode = ReadShader(fragmentFilename, "SHADER::Fragment", defineCode);
    std::string geometryCode;

    // Convert code into c-strings
    const char* vertexShaderSource = (vertexCode).c_str();
    const char* fragmentShaderSource = (fragmentCode).c_str();
    const char* geometryShaderSource;

    int success;
    char infoLog[512];
    
    // Create, attach source to shader, and compile for vertex and fragment shader
    vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);
    
    glCompileShader(vertexShaderID);
    glGetShaderiv(vertexShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShaderID, 1024, NULL, infoLog);
        LOG_ERROR("SHADER::VERTEX( " + vertexFilename + ")\n" + infoLog);
    }

    glCompileShader(fragmentShaderID);
    glGetShaderiv(fragmentShaderID, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShaderID, 1024, NULL, infoLog);
        LOG_ERROR("SHADER::Fragment(" + fragmentFilename + ")\n" + infoLog);
    }

    // Do all the above for geometry shader if supplied
    if (!geometryFilename.empty()) {
        geometryCode = ReadShader(geometryFilename, "SHADER::GEOMETRY", defineCode);
        geometryShaderSource = geometryCode.c_str();
        geometryShaderID = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShaderID, 1, &geometryShaderSource, NULL);
        glCompileShader(geometryShaderID);
        glGetShaderiv(geometryShaderID, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(geometryShaderID, 1024, NULL, infoLog);
            LOG_ERROR("SHADER::GEOMETRY( " + geometryFilename + ")\n" + infoLog);
        }
    }

    // Create, attach, and link shaders to program
    programID = glCreateProgram();
    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    if (!geometryFilename.empty()) {
        glAttachShader(programID, geometryShaderID);
    }
    glLinkProgram(programID);

    // Discard individual shaders
    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
    if (!geometryFilename.empty()) {
        glDeleteShader(geometryShaderID);
    }

    glEnable(GL_DEPTH_TEST);  
}

void Shader::Use() {
    glUseProgram(programID);
}

void Shader::Unload() {
    glDeleteProgram(programID);
}

void Shader::AddDefine(std::string define) {
    this->defines.push_back(define);
}

void Shader::SetInt(std::string location, int value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0) {
        glUniform1i(loc, value);
    }
}

void Shader::SetBool(std::string location, bool value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0) {
        glUniform1i(loc, (int)value);
    }
}

void Shader::SetFloat(std::string location, float value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0) {
        glUniform1f(loc, value);
    }
}

void Shader::SetVector(std::string location, glm::vec2 value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0)
        glUniform2fv(loc, 1, glm::value_ptr(value));
}

void Shader::SetVector(std::string location, glm::vec3 value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0)
        glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::SetVector(std::string location, glm::vec4 value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0)
        glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Shader::SetMatrix(std::string location, glm::mat2 value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0) {
        glUniformMatrix2fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::SetMatrix(std::string location, glm::mat3 value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0) {
        glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }
}

void Shader::SetMatrix(std::string location, glm::mat4 value) {
    unsigned int loc = glGetUniformLocation(programID, location.c_str());
    if (loc >= 0) {
        glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
    }
}
