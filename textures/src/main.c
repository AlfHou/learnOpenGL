#include <glad/glad.h>
// Glad needs to be before GLFW
#include "shader.h"
#include <GLFW/glfw3.h>
// TODO: Can this be removed now?
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Callback from GLFW that the window was resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
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

    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
        "Learning OpenGL all day", NULL, NULL);
    if (window == NULL) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return NULL;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return NULL;
    }

    return window;
}
unsigned int createShape(float* vertices, unsigned long sizeVertices,
    unsigned int* indices, unsigned long sizeIndices)
{
    // Create Vertex Array Object
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);

    // Get buffer id
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Get EBO buffer id
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    // Bind it before using VBO
    glBindVertexArray(VAO);

    // Bind buffer to a buffer type
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Copy vertices to GPU memory
    glBufferData(GL_ARRAY_BUFFER, sizeVertices, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeIndices, indices, GL_STATIC_DRAW);

    // Specify how opengl should interpret our verticies
    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    // Color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    return VAO;
}
unsigned int createTexture(unsigned int VAO)
{
    int width;
    int height;
    int nrChannels;
    unsigned char* data = stbi_load("../src/container.jpg", &width, &height,
        &nrChannels, 0);
    if (data == NULL) {
        fprintf(stderr, "Failed to load texture\n");
        return 0;
    }

    glBindVertexArray(VAO);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    stbi_image_free(data);
    return texture;
}

int main(void)
{
    GLFWwindow* window = setupWindow();
    if (window == NULL) {
        return 1;
    }

    // Set up vertices for first triangle
    float vertices[] = {
        // positions        // color    // texture cords
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,  // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, // bottom right
        -0.5f, -0.5, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5, 0.5, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f    // top left
    };
    unsigned int indices[] = {
        0, 1, 2, // First triangle
        0, 2, 3  // Second triangle
    };

    unsigned int VAO
        = createShape(vertices, sizeof(vertices), indices, sizeof(indices));

    unsigned int texture = createTexture(VAO);
    if (texture == 0) {
        glDeleteVertexArrays(1, &VAO);
        glfwTerminate();
        return 1;
    }

    struct shader s;
    shaderInit(&s, "../src/shader.vs", "../src/shader.fs");

    glBindVertexArray(0);
    // Render loop:
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(s.ID);

        glBindTexture(GL_TEXTURE_2D, texture);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
