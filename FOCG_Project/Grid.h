
#ifndef GRID_H
#define GRID_H

#include <OpenGL/gl3.h>
#include <glm/glm.hpp>

#include <map>
#include <tuple>

class Cube;

class Grid {
public:
    Grid();
    ~Grid();

    void draw(const glm::mat4& model,
              const glm::mat4& view,
              const glm::mat4& projection);

    void drawFilled(Cube& cube,
                    const glm::mat4& rotation,
                    const glm::mat4& view,
                    const glm::mat4& projection);

    void fill(int x, int y, int z, const glm::vec3& color);
    void clear(int x, int y, int z);
    bool isFilled(int x, int y, int z) const;

private:
    GLuint VAO;
    GLuint VBO;
    GLsizei vertexCount;

    std::map<std::tuple<int, int, int>, glm::vec3> filledCells;
};

#endif