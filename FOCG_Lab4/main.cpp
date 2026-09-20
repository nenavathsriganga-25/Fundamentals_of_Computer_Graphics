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

int windowWidth = 900;
int windowHeight = 700;

//stores cubes current rotation angles
float rotationX = 0.0f;
float rotationY = 0.0f;

// Read shader source code from a file
std::string readShaderFile(const char* filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to open shader file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    return buffer.str();
}

// Compile a shader
GLuint compileShader(GLenum type, const char* filename) {
    std::string source = readShaderFile(filename);

    if (source.empty())
        return 0;

    const char* shaderSource = source.c_str();
    GLuint shader = glCreateShader(type);

    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << "Shader compilation failed:\n" << infoLog << std::endl;
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

// Create and link the shader program
GLuint createShaderProgram() {
    GLuint vertexShader = compileShader(GL_VERTEX_SHADER, "vertex_shader.glsl");
    GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, "fragment_shader.glsl");

    if (vertexShader == 0 || fragmentShader == 0)
        return 0;

    GLuint program = glCreateProgram();

    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // Set the locations of vertex attributes
    glBindAttribLocation(program, 0, "aPos");
    glBindAttribLocation(program, 1, "aColor");

    glLinkProgram(program);

    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader linking failed:\n" << infoLog << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


// Handle keyboard input
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        // Press ESC to close the window
        if (key == GLFW_KEY_ESCAPE)
            glfwSetWindowShouldClose(window, true);

        // Rotate left
        if (key == GLFW_KEY_LEFT)
            rotationY -= 5.0f;

        // Rotate right
        if (key == GLFW_KEY_RIGHT)
            rotationY += 5.0f;

        // Rotate up
        if (key == GLFW_KEY_UP)
            rotationX -= 5.0f;

        // Rotate down
        if (key == GLFW_KEY_DOWN)
            rotationX += 5.0f;
    }
}

// Update viewport when the window is resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

int main() {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the window
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "FOCG Lab 4 - Color Cube", NULL, NULL);

    // Check if the window was created successfully
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    // Set the current OpenGL context
    glfwMakeContextCurrent(window);

    // Register keyboard and window resize callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Set viewport and enable depth testing
    glViewport(0, 0, windowWidth, windowHeight);
    glEnable(GL_DEPTH_TEST);

    // Create the shader program
    GLuint shaderProgram = createShaderProgram();

    // Check if shader program was created successfully
    if (shaderProgram == 0) {
        glfwTerminate();
        return -1;
    }

    // Vertex positions and 8 different vertex colors
    float vertices[] = {
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.5f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 1.0f
    };

    // Two triangles form each cube face
    unsigned int indices[] = {
        0, 1, 2,  0, 2, 3, // Front face
        4, 6, 5,  4, 7, 6, // Back face
        4, 0, 3,  4, 3, 7, // Left face
        1, 5, 6,  1, 6, 2, // Right face
        3, 2, 6,  3, 6, 7, // Top face
        4, 5, 1,  4, 1, 0 // Bottom face
    };

    // One light and unique color for each face
    glm::vec3 faceColors[6] = {
        glm::vec3(1.0f, 0.55f, 0.50f),  // Light Coral
        glm::vec3(0.70f, 0.45f, 1.0f),   // Light Purple
        glm::vec3(0.35f, 0.90f, 0.85f),  // Light Turquoise
        glm::vec3(1.0f, 0.75f, 0.35f),   // Light Orange
        glm::vec3(1.0f, 0.40f, 0.70f),   // Light Pink
        glm::vec3(0.65f, 1.0f, 0.40f)    // Light Lime
    };

    // Create VAO, VBO and EBO
    GLuint VAO, VBO, EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    // Bind the VAO
    glBindVertexArray(VAO);

    // Store vertex data in the VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Store index data in the EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Set vertex position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Set vertex color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind the VAO
    glBindVertexArray(0);

    // Get the locations of shader uniforms
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "MVP");
    GLint solidColorLoc = glGetUniformLocation(shaderProgram, "solidColor");
    GLint useVertexColorLoc = glGetUniformLocation(shaderProgram, "useVertexColor");

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Create model
        glm::mat4 model = glm::mat4(1.0f);

        // Apply rotation around the X-axis
        model = glm::rotate(model, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));

        // Apply rotation around the Y-axis
        model = glm::rotate(model, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));

        // Create the view matrix
        glm::mat4 view = glm::lookAt(
            glm::vec3(0.0f, 0.0f, 3.0f), // eye: camera position
            glm::vec3(0.0f, 0.0f, 0.0f), // center: what it's looking at
            glm::vec3(0.0f, 1.0f, 0.0f) // up: which way is "up"
        );

        // Create the projection matrix
        float aspect = (float)windowWidth / (float)windowHeight;
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

        // Calculate the final MVP matrix
        glm::mat4 MVP = projection * view * model;

        // Send the MVP matrix to the shader
        glUseProgram(shaderProgram);
        glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(MVP));

        glBindVertexArray(VAO);

        // Draw six solid-colored faces
        glUniform1i(useVertexColorLoc, 0);

        for (int i = 0; i < 6; i++) {
            glUniform3fv(solidColorLoc, 1, glm::value_ptr(faceColors[i]));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, (void*)(i * 6 * sizeof(unsigned int)));
        }

        // Draw the 8 colored vertices
        glUniform1i(useVertexColorLoc, 1);

        // Set the size of the colored vertex points
        glPointSize(30.0f);

        glDepthFunc(GL_LEQUAL);
        glDrawArrays(GL_POINTS, 0, 8);
        glDepthFunc(GL_LESS);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Delete OpenGL objects and clean up
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    // Destroy the window and terminate GLFW
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}