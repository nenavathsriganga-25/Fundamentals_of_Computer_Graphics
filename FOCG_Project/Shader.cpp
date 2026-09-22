
#define GL_SILENCE_DEPRECATION

#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile(vertexPath);
    std::ifstream fShaderFile(fragmentPath);

    if (!vShaderFile.is_open() || !fShaderFile.is_open()) {
        std::cerr << "Could not open shader files.\n";
        ID = 0;
        return;
    }

    std::stringstream vStream, fStream;
    vStream << vShaderFile.rdbuf();
    fStream << fShaderFile.rdbuf();

    vertexCode = vStream.str();
    fragmentCode = fStream.str();

    const char* vSource = vertexCode.c_str();
    const char* fSource = fragmentCode.c_str();

    GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vSource, nullptr);
    glCompileShader(vertex);

    GLint success;
    GLchar infoLog[1024];

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertex, 1024, nullptr, infoLog);
        std::cerr << "Vertex shader error:\n" << infoLog << '\n';
    }

    GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fSource, nullptr);
    glCompileShader(fragment);

    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragment, 1024, nullptr, infoLog);
        std::cerr << "Fragment shader error:\n" << infoLog << '\n';
    }

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    glBindAttribLocation(ID, 0, "position");
    glBindAttribLocation(ID, 1, "vertexColor");

    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 1024, nullptr, infoLog);
        std::cerr << "Shader linking error:\n" << infoLog << '\n';
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

void Shader::use() {
    if (ID != 0) {
        glUseProgram(ID);
    }
}