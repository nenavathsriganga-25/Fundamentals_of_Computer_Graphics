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
std::string readShaderFile(const char* fileName){
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

    glGetShaderiv(
        shader,
        GL_COMPILE_STATUS,
        &success
    );

    if (!success){
        glGetShaderInfoLog(
            shader,
            512,
            NULL,
            infoLog
        );
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

    glGetProgramiv(
        program,
        GL_LINK_STATUS,
        &success
    );

    if (!success){
        glGetProgramInfoLog(
            program,
            512,
            NULL,
            infoLog
        );
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

    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Create the window
    GLFWwindow* window =
        glfwCreateWindow(
            WIDTH,
            HEIGHT,
            "Lab 3 - Triangle and Rectangle",
            NULL,
            NULL
        );

    if (window == NULL){
        std::cerr << "Failed to create GLFW window"
                  << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make OpenGL context current
    glfwMakeContextCurrent(window);

    // Set viewport
    glViewport(
        0,
        0,
        WIDTH,
        HEIGHT
    );

    // Triangle vertices
    float triangleVertices[] ={
        -0.7f, -0.2f, 0.0f,
         0.0f,  0.6f, 0.0f,
         0.7f, -0.2f, 0.0f
    };

    // Rectangle vertices
    float rectangleVertices[] =  {
        -0.5f, -0.8f, 0.0f,
         0.5f, -0.8f, 0.0f,
         0.5f, -0.3f, 0.0f,
        -0.5f, -0.3f, 0.0f
    };

    // Read vertex shader
    std::string vertexCode = readShaderFile("vertex_shader.glsl");

    const char* vertexSource = vertexCode.c_str();

    // Create and compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource(
        vertexShader,
        1,
        &vertexSource,
        NULL
    );

    glCompileShader(vertexShader);

    checkShader( vertexShader, "Vertex");

    // Read fragment shader
    std::string fragmentCode = readShaderFile("fragment_shader.glsl");
    const char* fragmentSource = fragmentCode.c_str();

    // Create and compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(
        fragmentShader,
        1,
        &fragmentSource,
        NULL
    );

    glCompileShader(fragmentShader);
    checkShader( fragmentShader, "Fragment" );

    // Create shader program
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader( shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    // Link the shader
    glLinkProgram(shaderProgram);
    checkProgram(shaderProgram);


    // Individual shaders are no longer needed
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create VAO and VBO for triangle
    unsigned int triangleVAO;
    unsigned int triangleVBO;

    glGenVertexArrays( 1, &triangleVAO);
    glGenBuffers(1, &triangleVBO );

    // Bind triangle VAO
    glBindVertexArray( triangleVAO );

    // Bind triangle VBO
    glBindBuffer( GL_ARRAY_BUFFER, triangleVBO);


    // Copy triangle data to GPU
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(triangleVertices),
        triangleVertices,
        GL_STATIC_DRAW
    );


    // Position attribute
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    //Create VAO and VBO for rectangle
    unsigned int rectangleVAO;
    unsigned int rectangleVBO;

    glGenVertexArrays( 1, &rectangleVAO );
    glGenBuffers( 1, &rectangleVBO);

    // Bind rectangle VAO
    glBindVertexArray(rectangleVAO);

    // Bind rectangle VBO
    glBindBuffer(GL_ARRAY_BUFFER, rectangleVBO);


    // Copy rectangle data to GPU
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(rectangleVertices),
        rectangleVertices,
        GL_STATIC_DRAW
    );


    // Position attribute
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    //Rendering loop
    while (!glfwWindowShouldClose(window))  {
        // Background color
        glClearColor(
            0.1f,
            0.1f,
            0.1f,
            1.0f
        );

        glClear( GL_COLOR_BUFFER_BIT );

        // Use shader program
        glUseProgram(shaderProgram);

        glUniform4f(
            glGetUniformLocation(shaderProgram, "ourColor"),
            1.0f,
            0.0f,
            0.0f,
            1.0f
        );

        // Bind triangle VAO
        glBindVertexArray(triangleVAO);

        // Draw triangle
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Draw rectangle border
        glUniform4f(
            glGetUniformLocation(shaderProgram, "ourColor"),
            1.0f,
            1.0f,
            1.0f,
            1.0f
        );

        glBindVertexArray( rectangleVAO);

        // Draw only the border
        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    // Cleanup
    glDeleteVertexArrays(1, &triangleVAO  );

    glDeleteBuffers(1, &triangleVBO);

    glDeleteVertexArrays(1, &rectangleVAO  );

    glDeleteBuffers(1, &rectangleVBO);

    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
