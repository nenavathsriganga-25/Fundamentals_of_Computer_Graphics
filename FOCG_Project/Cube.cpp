
#define GL_SILENCE_DEPRECATION

#include "Cube.h"

#include <glm/gtc/type_ptr.hpp>

Cube::Cube() : VAO(0), VBO(0) {
    float vertices[] = {
        // Front
        0,0,1, 1,0,1, 1,1,1,
        0,0,1, 1,1,1, 0,1,1,

        // Back
        0,0,0, 1,1,0, 1,0,0,
        0,0,0, 0,1,0, 1,1,0,

        // Left
        0,0,0, 0,0,1, 0,1,1,
        0,0,0, 0,1,1, 0,1,0,

        // Right
        1,0,0, 1,1,1, 1,0,1,
        1,0,0, 1,1,0, 1,1,1,

        // Top
        0,1,0, 0,1,1, 1,1,1,
        0,1,0, 1,1,1, 1,1,0,

        // Bottom
        0,0,0, 1,0,1, 0,0,1,
        0,0,0, 1,0,0, 1,0,1
    };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices),
                 vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

Cube::~Cube() {
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
}

void Cube::draw(const glm::mat4& model,
                const glm::mat4& view,
                const glm::mat4& projection,
                const glm::vec3& color,
                float alpha,
                float glow) {
    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    glUniformMatrix4fv(
        glGetUniformLocation(program, "model"),
        1, GL_FALSE, glm::value_ptr(model));

    glUniformMatrix4fv(
        glGetUniformLocation(program, "view"),
        1, GL_FALSE, glm::value_ptr(view));

    glUniformMatrix4fv(
        glGetUniformLocation(program, "projection"),
        1, GL_FALSE, glm::value_ptr(projection));

    glUniform3fv(
        glGetUniformLocation(program, "objectColor"),
        1, glm::value_ptr(color));

    glUniform1i(
        glGetUniformLocation(program, "useSolidColor"), 1);

    glUniform1i(
        glGetUniformLocation(program, "useLighting"), 1);

    glUniform1f(
        glGetUniformLocation(program, "alpha"), alpha);

    glUniform1f(
        glGetUniformLocation(program, "glow"), glow);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}