
#define GL_SILENCE_DEPRECATION

#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <cstddef>

int windowWidth = 900;
int windowHeight = 700;

float rotationX = 0.0f;
float rotationY = 0.0f;

GLuint shaderProgram;
GLuint faceVAO, faceVBO;
GLuint nodeVAO, nodeVBO;

GLint mvpLoc;

// Stores the position and color of a vertex
struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
};

// Stores the four corner nodes and color of a cube face
struct Face {
    int nodes[4];
    glm::vec3 color;
};

// Initial colors of the eight cube nodes
glm::vec3 nodeColors[8] = {
    {1.0f, 0.0f, 0.0f}, // 0 - Red
    {0.0f, 1.0f, 0.0f}, // 1 - Green
    {0.0f, 0.0f, 1.0f}, // 2 - Blue
    {1.0f, 1.0f, 0.0f}, // 3 - Yellow
    {1.0f, 0.0f, 1.0f}, // 4 - Magenta
    {0.0f, 1.0f, 1.0f}, // 5 - Cyan
    {1.0f, 0.5f, 0.0f}, // 6 - Orange
    {0.5f, 0.0f, 1.0f}  // 7 - Purple
};

// Slightly lighter face colors
Face faces[6] = {
    {{0, 1, 2, 3}, {1.0f, 0.25f, 0.25f}}, 
    {{4, 5, 6, 7}, {0.25f, 1.0f, 0.25f}}, 
    {{4, 0, 3, 7}, {0.35f, 0.35f, 1.0f}}, 
    {{1, 5, 6, 2}, {1.0f, 1.0f, 0.35f}}, 
    {{3, 2, 6, 7}, {1.0f, 0.35f, 1.0f}}, 
    {{4, 5, 1, 0}, {0.35f, 1.0f, 1.0f}}  
};

// Defines the 3D coordinates of the eight cube nodes
glm::vec3 nodePositions[8] = {
    {-0.5f, -0.5f,  0.5f},
    { 0.5f, -0.5f,  0.5f},
    { 0.5f,  0.5f,  0.5f},
    {-0.5f,  0.5f,  0.5f},
    {-0.5f, -0.5f, -0.5f},
    { 0.5f, -0.5f, -0.5f},
    { 0.5f,  0.5f, -0.5f},
    {-0.5f,  0.5f, -0.5f}
};

// Stores vertex data for cube faces and node points
std::vector<Vertex> faceVertices;
std::vector<Vertex> nodeVertices;

// Reads the contents of a shader file and returns it as a string
std::string readShaderFile(const char* filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Could not open shader file: "
                  << filename << '\n';
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Compiles a vertex or fragment shader
GLuint compileShader(GLenum type, const char* filename) {
    std::string source = readShaderFile(filename);

    if (source.empty()) {
        return 0;
    }

    const char* sourcePtr = source.c_str();
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &sourcePtr, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader compilation failed:\n"
                  << infoLog << '\n';
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint createShaderProgram() {
    GLuint vertexShader =
        compileShader(GL_VERTEX_SHADER, "vertex_shader.glsl");

    GLuint fragmentShader =
        compileShader(GL_FRAGMENT_SHADER, "fragment_shader.glsl");

    if (vertexShader == 0 || fragmentShader == 0) {
        return 0;
    }

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glBindAttribLocation(program, 0, "aPos");
    glBindAttribLocation(program, 1, "aColor");

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "Shader linking failed:\n"
                  << infoLog << '\n';
        glDeleteProgram(program);
        program = 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

// Creates the vertex data for all six cube faces and eight node points
void createGeometry() {
    faceVertices.clear();
    nodeVertices.clear();

    // Each face has two triangles with the same initial face color.
    for (int f = 0; f < 6; f++) {
        int a = faces[f].nodes[0];
        int b = faces[f].nodes[1];
        int c = faces[f].nodes[2];
        int d = faces[f].nodes[3];

        glm::vec3 color = faces[f].color;

        faceVertices.push_back({nodePositions[a], color});
        faceVertices.push_back({nodePositions[b], color});
        faceVertices.push_back({nodePositions[c], color});

        faceVertices.push_back({nodePositions[a], color});
        faceVertices.push_back({nodePositions[c], color});
        faceVertices.push_back({nodePositions[d], color});
    }

    // Separate vertices for the eight node points.
    for (int i = 0; i < 8; i++) {
        nodeVertices.push_back({
            nodePositions[i],
            nodeColors[i]
        });
    }
}

// Creates VAOs and VBOs and sends vertex data to the GPU
void setupBuffers() {
    glGenVertexArrays(1, &faceVAO);
    glGenBuffers(1, &faceVBO);

    glBindVertexArray(faceVAO);
    glBindBuffer(GL_ARRAY_BUFFER, faceVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        faceVertices.size() * sizeof(Vertex),
        faceVertices.data(),
        GL_DYNAMIC_DRAW
    );

    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)0
    );
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)offsetof(Vertex, color)
    );
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &nodeVAO);
    glGenBuffers(1, &nodeVBO);

    glBindVertexArray(nodeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, nodeVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        nodeVertices.size() * sizeof(Vertex),
        nodeVertices.data(),
        GL_DYNAMIC_DRAW
    );

    // Specify the position attribute of each vertex
    glVertexAttribPointer(
        0, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)0
    );
    glEnableVertexAttribArray(0);

    // Specify the color attribute of each vertex
    glVertexAttribPointer(
        1, 3, GL_FLOAT, GL_FALSE,
        sizeof(Vertex), (void*)offsetof(Vertex, color)
    );
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

// Updates the selected node color and the corresponding face vertices
void updateNodeColor(int nodeIndex, float r, float g, float b) {
    if (nodeIndex < 0 || nodeIndex > 7) {
        std::cout << "Enter a node number from 0 to 7.\n";
        return;
    }

    // Check whether the entered node index is valid
    if (r < 0.0f || r > 1.0f ||
        g < 0.0f || g > 1.0f ||
        b < 0.0f || b > 1.0f) {
        std::cout << "RGB values must be between 0 and 1.\n";
        return;
    }

    glm::vec3 newColor(r, g, b);
    nodeColors[nodeIndex] = newColor;

    // Update all copies of this node in the face triangles.
    for (int f = 0; f < 6; f++) {
        int firstVertex = f * 6;

        for (int corner = 0; corner < 4; corner++) {
            if (faces[f].nodes[corner] == nodeIndex) {
                if (corner == 0) {
                    faceVertices[firstVertex].color = newColor;
                    faceVertices[firstVertex + 3].color = newColor;
                }
                else if (corner == 1) {
                    faceVertices[firstVertex + 1].color = newColor;
                }
                else if (corner == 2) {
                    faceVertices[firstVertex + 2].color = newColor;
                    faceVertices[firstVertex + 4].color = newColor;
                }
                else if (corner == 3) {
                    faceVertices[firstVertex + 5].color = newColor;
                }
            }
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, faceVBO);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        0,
        faceVertices.size() * sizeof(Vertex),
        faceVertices.data()
    );

    // Update the visible node point.
    nodeVertices[nodeIndex].color = newColor;

    glBindBuffer(GL_ARRAY_BUFFER, nodeVBO);
    glBufferSubData(
        GL_ARRAY_BUFFER,
        nodeIndex * sizeof(Vertex),
        sizeof(Vertex),
        &nodeVertices[nodeIndex]
    );

    std::cout << "Node " << nodeIndex
              << " color updated.\n";
}

// Handles keyboard input for cube rotation and node color changes
void key_callback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods
) {
    if (action != GLFW_PRESS && action != GLFW_REPEAT) {
        return;
    }

    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, true);
    }

    if (key == GLFW_KEY_LEFT) {
        rotationY -= 5.0f;
    }

    if (key == GLFW_KEY_RIGHT) {
        rotationY += 5.0f;
    }

    if (key == GLFW_KEY_UP) {
        rotationX -= 5.0f;
    }

    if (key == GLFW_KEY_DOWN) {
        rotationX += 5.0f;
    }

    if (key == GLFW_KEY_I && action == GLFW_PRESS) {
        int nodeIndex;
        float r, g, b;

        std::cout << "\nEnter node number (0-7): ";

        if (!(std::cin >> nodeIndex)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            std::cout << "Invalid node input.\n";
            return;
        }

        std::cout << "Enter R value: ";
        if (!(std::cin >> r)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            std::cout << "Invalid R value.\n";
            return;
        }

        std::cout << "Enter G value: ";
        if (!(std::cin >> g)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            std::cout << "Invalid G value.\n";
            return;
        }

        std::cout << "Enter B value: ";
        if (!(std::cin >> b)) {
            std::cin.clear();
            std::cin.ignore(
                std::numeric_limits<std::streamsize>::max(), '\n'
            );
            std::cout << "Invalid B value.\n";
            return;
        }

        updateNodeColor(nodeIndex, r, g, b);
    }
}

void framebuffer_size_callback(
    GLFWwindow* window,
    int width,
    int height
) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW.\n";
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(
        windowWidth,
        windowHeight,
        "FOCG Lab 5 - Color Interpolation",
        nullptr,
        nullptr
    );

    if (!window) {
        std::cerr << "Failed to create GLFW window.\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);

    shaderProgram = createShaderProgram();

    if (shaderProgram == 0) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    createGeometry();
    setupBuffers();

    glUseProgram(shaderProgram);
    mvpLoc = glGetUniformLocation(shaderProgram, "MVP");

    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model(1.0f);

        model = glm::rotate(
            model,
            glm::radians(rotationX),
            glm::vec3(1.0f, 0.0f, 0.0f)
        );

        model = glm::rotate(
            model,
            glm::radians(rotationY),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f),
            glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        glm::mat4 projection = glm::perspective(
            glm::radians(45.0f),
            (float)windowWidth / (float)windowHeight,
            0.1f,
            100.0f
        );

        glm::mat4 MVP = projection * view * model;

        glUseProgram(shaderProgram);
        glUniformMatrix4fv(
            mvpLoc,
            1,
            GL_FALSE,
            glm::value_ptr(MVP)
        );

        // Draw the six faces.
        glBindVertexArray(faceVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Draw the eight colored node points.
        glBindVertexArray(nodeVAO);
        glPointSize(30.0f);
        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_POINTS, 0, 8);
        glDepthFunc(GL_LESS);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &faceVAO);
    glDeleteBuffers(1, &faceVBO);
    glDeleteVertexArrays(1, &nodeVAO);
    glDeleteBuffers(1, &nodeVBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}