#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>

int main()
{
if (!glfwInit())
{
std::cout << "Failed to initialize GLFW!" << std::endl;
return -1;
}

GLFWwindow* window = glfwCreateWindow(
800,
600,
"OpenGL Version",
NULL,
NULL);

if (!window)
{
std::cout << "Failed to create window!" << std::endl;
glfwTerminate();
return -1;
}

glfwMakeContextCurrent(window);

glewExperimental = GL_TRUE;

GLenum err = glewInit();

if (err != GLEW_OK)
{
std::cout << "GLEW Initialization Failed!" << std::endl;
glfwTerminate();
return -1;
}

const char* openGLVersion =
(const char*)glGetString(GL_VERSION);

const char* glewVersion =
(const char*)glewGetString(GLEW_VERSION);

const char* glfwVersion =
glfwGetVersionString();

std::cout << "OpenGL Version : "
<< openGLVersion << std::endl;

std::cout << "GLEW Version : "
<< glewVersion << std::endl;

std::cout << "GLFW Version : "
<< glfwVersion << std::endl;

std::string title =
"OpenGL: " + std::string(openGLVersion) +
" | GLEW: " + std::string(glewVersion) +
" | GLFW: " + std::string(glfwVersion);

glfwSetWindowTitle(window, title.c_str());

glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

while (!glfwWindowShouldClose(window))
{
glClear(GL_COLOR_BUFFER_BIT);

glfwSwapBuffers(window);
glfwPollEvents();
}

glfwTerminate();

return 0;
}

