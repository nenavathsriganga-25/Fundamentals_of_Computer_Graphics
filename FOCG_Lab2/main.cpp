#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define GL_SILENCE_DEPRECATION
#include <OpenGL/gl3.h>
#include <GLFW/glfw3.h>

const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;

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

void checkShader(unsigned int shader, const char* type)
{
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success){
        glGetShaderInfoLog( shader, 512, NULL, infoLog );
        std::cerr << type
                  << " shader compilation failed:\n"
                  << infoLog
                  << std::endl;
    }
}

void checkProgram(unsigned int program)
{
    int success;
    char infoLog[512];

    glGetProgramiv( program, GL_LINK_STATUS, &success );

    if (!success){
        glGetProgramInfoLog( program, 512, NULL, infoLog );
        std::cerr << "Shader program linking failed:\n"
                  << infoLog
                  << std::endl;
    }
}

int main()
{
    // Initialize GLFW
    if (!glfwInit()){
        std::cerr << "Failed to initialize GLFW"
                  << std::endl;
        return -1;
    }
    
    // Set OpenGL version and profile
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );

    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 2 );

    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );

    // Creating the window
    GLFWwindow* window = glfwCreateWindow( WIDTH, HEIGHT, "RGB Triangle", NULL, NULL );

    if (window == NULL){
        std::cerr << "Failed to create GLFW window"
                  << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's OpenGL context current
    glfwMakeContextCurrent(window);

    // Set the viewport
    glViewport(0, 0, WIDTH, HEIGHT);

    float vertices[] =
    {
        // Position              // Color
         0.0f,  0.5f, 0.0f,      0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f,      1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,      0.0f, 0.0f, 1.0f
    };

    // Read the vertex shader
    std::string vertexCode = readShaderFile("vertex_shader.glsl");
    const char* vertexSource = vertexCode.c_str();

    // Create and compile the vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

    glShaderSource( vertexShader, 1, &vertexSource, NULL );
    glCompileShader(vertexShader);
    checkShader( vertexShader, "Vertex" );

    // Read the fragment shader
    std::string fragmentCode = readShaderFile("fragment_shader.glsl");
    const char* fragmentSource = fragmentCode.c_str();

    // Create and compile the fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource( fragmentShader, 1, &fragmentSource, NULL );
    glCompileShader(fragmentShader);
    checkShader( fragmentShader,"Fragment");

    // Create the shader program
    unsigned int shaderProgram = glCreateProgram();

    glAttachShader( shaderProgram, vertexShader );
    glAttachShader( shaderProgram, fragmentShader);

     // Assign locations to vertex attributes
    glBindAttribLocation( shaderProgram, 0, "aPos" );

    glBindAttribLocation( shaderProgram,1, "aColor");


    // Link the shaders into one program
    glLinkProgram(shaderProgram);
    checkProgram(shaderProgram);

    // Delete individual shaders after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Create VAO and VBO
    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays( 1, &VAO);
    glGenBuffers( 1, &VBO );

    // Bind VAO
    glBindVertexArray(VAO);

    // Bind VBO
    glBindBuffer( GL_ARRAY_BUFFER, VBO );

    // Copy vertex data into GPU memory
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices,  GL_STATIC_DRAW );

    //position attribute 
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    //color attribute
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    glEnableVertexAttribArray(1);


    //rendering loop
    while (!glfwWindowShouldClose(window))
    {
        // Background
        glClearColor( 0.1f, 0.1f, 0.1f, 1.0f );

         glClear( GL_COLOR_BUFFER_BIT );

        // Use shader program
        glUseProgram( shaderProgram );

        // Bind VAO
        glBindVertexArray( VAO );

        // Draw triangle
        glDrawArrays( GL_TRIANGLES, 0, 3 );

        // Display the result
        glfwSwapBuffers(window);

        // Process events
        glfwPollEvents();
    }


   //cleanup
    glDeleteVertexArrays( 1, &VAO );

    glDeleteBuffers( 1, &VBO );

    glDeleteProgram( shaderProgram );

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
