
#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <limits>
#include <algorithm>

#include "Cube.h"
#include "Grid.h"
#include "Shader.h"

namespace {
    constexpr int GRID_SIZE = 5;
}

int cubeX = 0;
int cubeY = 0;
int cubeZ = 0;

glm::vec3 cubeColor(1.0f, 0.65f, 0.1f);

float rotateX = 0.0f;
float rotateY = 0.0f;

Grid* gridPtr = nullptr;

void key_callback(GLFWwindow* window,
                  int key,
                  int scancode,
                  int action,
                  int mods) {
    if (action != GLFW_PRESS) {
        return;
    }

    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        break;

    // Move along X
    case GLFW_KEY_LEFT:
        cubeX = std::max(0, cubeX - 1);
        break;

    case GLFW_KEY_RIGHT:
        cubeX = std::min(GRID_SIZE - 1, cubeX + 1);
        break;

    // Move along Y
    case GLFW_KEY_UP:
        cubeY = std::min(GRID_SIZE - 1, cubeY + 1);
        break;

    case GLFW_KEY_DOWN: cubeY = std::max(0, cubeY - 1);
        break;

    // Move along Z
    case GLFW_KEY_U: cubeZ = std::min(GRID_SIZE - 1, cubeZ + 1);
        break;

    case GLFW_KEY_B: cubeZ = std::max(0, cubeZ - 1);
        break;

    // Rotate the complete grid and its contents
    case GLFW_KEY_L: rotateY -= 15.0f;
        break;

    case GLFW_KEY_R: rotateY += 15.0f;
        break;

    case GLFW_KEY_T: rotateX -= 15.0f;
        break;

    case GLFW_KEY_D: rotateX += 15.0f;
        break;

    // Change RGB color; only this key requests terminal input
    case GLFW_KEY_C: {
        std::cout << "\nEnter RGB values (0-1 or 0-255): "
                  << std::flush;

        float r, g, b;

        if (!(std::cin >> r >> g >> b)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(),
                '\n'
            );

            std::cout << "Invalid input. Enter three numbers.\n";
            break;
        }

        // If any component is above 1, treat input as 0-255 RGB.
        if (r > 1.0f || g > 1.0f || b > 1.0f) {
            r /= 255.0f;
            g /= 255.0f;
            b /= 255.0f;
        }

        cubeColor = glm::clamp(
            glm::vec3(r, g, b),
            glm::vec3(0.0f),
            glm::vec3(1.0f)
        );

        std::cout << "Color updated.\n";
        break;
    }

    // Fill the selected cell
    case GLFW_KEY_F:
        if (gridPtr) {
            gridPtr->fill(cubeX, cubeY, cubeZ, cubeColor);
        }
        break;

    // Clear the selected cell
    case GLFW_KEY_W:
        if (gridPtr) {
            gridPtr->clear(cubeX, cubeY, cubeZ);
        }
        break;
    }
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        1100, 850,
        "FOCG - 3D Grid Volume",
        nullptr,
        nullptr
    );

    if (!window) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetKeyCallback(window, key_callback);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    Shader shader(
        "vertex_shader.glsl",
        "fragment_shader.glsl"
    );

    if (shader.ID == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    Cube cube;
    Grid grid;

    gridPtr = &grid;

    while (!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);

        if (height == 0) {
            height = 1;
        }

        glViewport(0, 0, width, height);

        glClearColor(0.07f, 0.04f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.use();

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            static_cast<float>(width) / static_cast<float>(height),
            0.1f,
            100.0f
        );

        glm::mat4 view = glm::lookAt(
            glm::vec3(10.5f, 10.0f, 12.5f),
            glm::vec3(2.5f, 2.5f, 2.5f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Rotate around the center of the complete 5x5x5 volume
        glm::mat4 rotation(1.0f);

        rotation = glm::translate(
            rotation,
            glm::vec3(2.5f, 2.5f, 2.5f)
        );

        rotation = glm::rotate(
            rotation,
            glm::radians(rotateX),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        rotation = glm::rotate(
            rotation,
            glm::radians(rotateY),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        rotation = glm::translate(
            rotation,
            glm::vec3(-2.5f, -2.5f, -2.5f)
        );

        // Draw persistent filled cells with the same rotation
        grid.drawFilled(
            cube,
            rotation,
            view,
            projection
        );

        // Draw the selected cube in its current cell
        if (!grid.isFilled(cubeX, cubeY, cubeZ)) {
            glm::mat4 cubeModel = glm::translate(
                rotation,
                glm::vec3(cubeX, cubeY, cubeZ)
            );

            cube.draw(
                cubeModel,
                view,
                projection,
                cubeColor,
                1.0f,
                0.0f
            );
        }

        // Draw the entire 3D lattice using that same rotation
        grid.draw(
            rotation,
            view,
            projection
        );

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    gridPtr = nullptr;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}