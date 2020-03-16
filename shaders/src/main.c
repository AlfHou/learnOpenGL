#include <glad/glad.h>
// Glad needs to be before GLFW
#include "shader.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Callback from GLFW that the window was resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
GLFWwindow* setupWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Learning OpenGL all day", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return NULL;
    }

    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    return window;
}
unsigned int createTriangle(float* vertices, unsigned long sizeVertices)
{
    // Create Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Get buffer id
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Bind it before using VBO
    glBindVertexArray(VAO);

    // Bind buffer to a buffer type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy vertices to GPU memory
    glBufferData(GL_ARRAY_BUFFER, sizeVertices, vertices, GL_STATIC_DRAW);

    // Specify how opengl should interpret our verticies
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}

int main(void)
{
    GLFWwindow* window = setupWindow();
    if (window == NULL) {
        return -1;
    }

    // Set up vertices for first triangle
    float vertices[] = {
        // positions        // color
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // Lower left
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,  // Lower right
        0.0f, 0.65, 0.0f, 0.0f, 0.0f, 1.0f    // Top
    };

    unsigned int VAO
        = createTriangle(vertices, sizeof(vertices));

    struct shader s;
    shaderInit(&s, "../src/shader.vs", "../src/shader.fs");

    // Render loop:
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(s.ID);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
