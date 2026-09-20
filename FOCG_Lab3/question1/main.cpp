#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;


// Read shader code from a file
std::string readShaderFile(const char* fileName)
{
    std::ifstream file(fileName);

    if (!file.is_open()){
        std::cerr << "Could not open shader file: "
                  << fileName << std::endl;
        exit(EXIT_FAILURE);
    }

    std::stringstream buffer;

    buffer << file.rdbuf();

    return buffer.str();
}

// Check shader compilation
void checkShader(unsigned int shader, const char* type){
    int success;
    char infoLog[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success){
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cerr << type
                  << " shader compilation failed:\n"
                  << infoLog
                  << std::endl;
    }
}

// Check shader program linking
void checkProgram(unsigned int program){
    int success;
    char infoLog[512];

    glGetProgramiv(program, GL_LINK_STATUS, &success);

    if (!success){
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cerr << "Shader program linking failed:\n"
                  << infoLog
                  << std::endl;
    }
}


int main(){
    // Initialize GLFW
    if (!glfwInit()){
        std::cerr << "Failed to initialize GLFW"
                  << std::endl;
        return -1;
    }

    // Set OpenGL version and profile
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,2 );

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);


    // Create the window
    GLFWwindow* window =
        glfwCreateWindow(WIDTH, HEIGHT, "Four Colored Rectangle", NULL, NULL);

    if (window == NULL){
        std::cerr << "Failed to create GLFW window"
                  << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make OpenGL context current

    glfwMakeContextCurrent(window);


    // Set viewport

    glViewport(0, 0, WIDTH, HEIGHT);

    // Define four vertices

    float vertices[] ={

        // A - Top Left 
        -0.5f,  0.5f, 0.0f,
         1.0f,  0.0f, 0.0f,

        // B - Top Right 
         0.5f,  0.5f, 0.0f,
         0.0f,  1.0f, 0.0f,

        // C - Bottom Right 
         0.5f, -0.5f, 0.0f,
         0.0f,  0.0f, 1.0f,

        // D - Bottom Left 
        -0.5f, -0.5f, 0.0f,
         1.0f,  1.0f, 0.0f
    };

    // Define indices for two triangles
    /* Triangle 1 = A B C ,  Triangle 2 = A C D */

    unsigned int indices[] ={
        0, 1, 2,
        0, 2, 3
    };

    // Read vertex shader

    std::string vertexCode = readShaderFile("vertex_shader.glsl");

    const char* vertexSource = vertexCode.c_str();

    // Create and compile vertex shader

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(vertexShader, 1, &vertexSource, NULL);

    glCompileShader(vertexShader);

    checkShader(vertexShader, "Vertex"
);

    // Read fragment shader

    std::string fragmentCode = readShaderFile("fragment_shader.glsl");

    const char* fragmentSource = fragmentCode.c_str();

    // Create and compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);

    glCompileShader(fragmentShader);

    checkShader(fragmentShader, "Fragment");

    // Create shader program
    unsigned int shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);

    glAttachShader(shaderProgram, fragmentShader);

    // Assign attribute locations
    glBindAttribLocation(shaderProgram, 0, "aPos");

    glBindAttribLocation(shaderProgram,1,"aColor");


    // Link the shaders
    glLinkProgram(shaderProgram);
    checkProgram(shaderProgram);

    // Individual shaders are no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create VAO, VBO and EBO

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    // Create VAO
    glGenVertexArrays(1, &VAO);

    // Create VBO
    glGenBuffers(1, &VBO);

    // Create EBO
    glGenBuffers(1, &EBO);

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO and copy vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Bind EBO and copy index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(0);

    // Color attribute
    glVertexAttribPointer(
        1, 
        3,
        GL_FLOAT, 
        GL_FALSE, 
        6 * sizeof(float), 
        (void*)(3 * sizeof(float)));

    glEnableVertexAttribArray(1);

    // Rendering loop

    while (!glfwWindowShouldClose(window)){
        // Background color
        glClearColor(
            0.1f,
            0.1f,
            0.1f,
            1.0f
        );

        glClear(GL_COLOR_BUFFER_BIT);

        // Use shader program
        glUseProgram(shaderProgram);

        // Bind VAO
        glBindVertexArray(VAO);

        // Draw two triangles using the indices
        glDrawElements(
            GL_TRIANGLES,
            6,
            GL_UNSIGNED_INT,
            0
        );

        // Display the result
        glfwSwapBuffers(window);

        // Process events
        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &VAO);

    glDeleteBuffers(1, &VBO);

    glDeleteBuffers(1, &EBO);

    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
