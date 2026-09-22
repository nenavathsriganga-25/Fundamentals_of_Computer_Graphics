#ifndef SHADER_H
#define SHADER_H

#include <OpenGL/gl3.h>
#include <string>

class Shader {
public:
    GLuint ID;

    Shader(const char* vertexPath, const char* fragmentPath);
    void use();
};

#endif