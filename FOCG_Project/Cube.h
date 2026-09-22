
#ifndef CUBE_H
#define CUBE_H

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>

class Cube {
public:
    Cube();
    ~Cube();

    void draw(const glm::mat4& model,
              const glm::mat4& view,
              const glm::mat4& projection,
              const glm::vec3& color,
              float alpha = 1.0f,
              float glow = 0.0f);

private:
    GLuint VAO, VBO;
};

#endif