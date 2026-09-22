
#define GL_SILENCE_DEPRECATION

#include "Grid.h"
#include "Cube.h"

#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace {
    constexpr int GRID_SIZE = 5;
}

Grid::Grid() : VAO(0), VBO(0), vertexCount(0) {
    std::vector<float> vertices;

    const float R = 0.48f;
    const float G = 0.58f;
    const float B = 0.72f;

    auto addLine = [&](float x1, float y1, float z1,
                       float x2, float y2, float z2) {
        vertices.insert(vertices.end(), {
            x1, y1, z1, R, G, B,
            x2, y2, z2, R, G, B
        });
    };

    // Lines parallel to X
    for (int y = 0; y <= GRID_SIZE; y++) {
        for (int z = 0; z <= GRID_SIZE; z++) {
            addLine(
                0, y, z,
                GRID_SIZE, y, z
            );
        }
    }

    // Lines parallel to Y
    for (int x = 0; x <= GRID_SIZE; x++) {
        for (int z = 0; z <= GRID_SIZE; z++) {
            addLine(
                x, 0, z,
                x, GRID_SIZE, z
            );
        }
    }

    // Lines parallel to Z
    for (int x = 0; x <= GRID_SIZE; x++) {
        for (int y = 0; y <= GRID_SIZE; y++) {
            addLine(
                x, y, 0,
                x, y, GRID_SIZE
            );
        }
    }

    vertexCount = static_cast<GLsizei>(vertices.size() / 6);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size() * sizeof(float),
        vertices.data(),
        GL_STATIC_DRAW
    );

    GLsizei stride = 6 * sizeof(float);

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        stride, nullptr
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        stride, (void*)(3 * sizeof(float))
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

Grid::~Grid() {
    if (VBO != 0) {
        glDeleteBuffers(1, &VBO);
    }

    if (VAO != 0) {
        glDeleteVertexArrays(1, &VAO);
    }
}

void Grid::fill(int x, int y, int z, const glm::vec3& color) {
    if (x < 0 || x >= GRID_SIZE ||
        y < 0 || y >= GRID_SIZE ||
        z < 0 || z >= GRID_SIZE) {
        return;
    }

    filledCells[{x, y, z}] = color;
}

void Grid::clear(int x, int y, int z) {
    filledCells.erase({x, y, z});
}

bool Grid::isFilled(int x, int y, int z) const {
    return filledCells.find({x, y, z}) != filledCells.end();
}

void Grid::draw(const glm::mat4& model,
                const glm::mat4& view,
                const glm::mat4& projection) {
    GLint program = 0;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    glUniformMatrix4fv(
        glGetUniformLocation(program, "model"),
        1, GL_FALSE, glm::value_ptr(model)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(program, "view"),
        1, GL_FALSE, glm::value_ptr(view)
    );

    glUniformMatrix4fv(
        glGetUniformLocation(program, "projection"),
        1, GL_FALSE, glm::value_ptr(projection)
    );

    glUniform1i(
        glGetUniformLocation(program, "useSolidColor"), 0
    );

    glUniform1i(
        glGetUniformLocation(program, "useLighting"), 0
    );

    glUniform1f(
        glGetUniformLocation(program, "alpha"), 1.0f
    );

    glUniform1f(
        glGetUniformLocation(program, "glow"), 0.0f
    );

    glBindVertexArray(VAO);

    glLineWidth(3.0f);
    glDrawArrays(GL_LINES, 0, vertexCount);
    glLineWidth(1.0f);

    glBindVertexArray(0);
}

void Grid::drawFilled(Cube& cube,
                      const glm::mat4& rotation,
                      const glm::mat4& view,
                      const glm::mat4& projection) {
    for (const auto& cell : filledCells) {
        int x = std::get<0>(cell.first);
        int y = std::get<1>(cell.first);
        int z = std::get<2>(cell.first);

        glm::mat4 cellModel = glm::translate(
            rotation,
            glm::vec3(x, y, z)
        );

        cube.draw(
            cellModel,
            view,
            projection,
            cell.second,
            1.0f,
            0.0f
        );
    }
}