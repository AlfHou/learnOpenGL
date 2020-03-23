#include <glad/glad.h>
// Glad needs to be before GLFW
#include "shader.h"
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cglm/cglm.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define NUM_CUBES 10

// Callback from GLFW that the window was resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window, float* sampleRatio,
    struct shader* const s)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (*sampleRatio < 1.0) {
            *sampleRatio = *sampleRatio + 0.01;
            glUniform1f(glGetUniformLocation(s->ID, "sampleRatio"),
                *sampleRatio);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (*sampleRatio > 0) {
            *sampleRatio = *sampleRatio - 0.01;
            glUniform1f(glGetUniformLocation(s->ID, "sampleRatio"),
                *sampleRatio);
        }
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
unsigned int createShape(float* vertices, unsigned long sizeVertices)
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}
unsigned int createTexture(unsigned int VAO, char* const image_path,
    GLint imageFormat)
{
    stbi_set_flip_vertically_on_load(true);

    int width;
    int height;
    int nrChannels;
    unsigned char* data = stbi_load(image_path, &width, &height,
        &nrChannels, 0);
    if (data == NULL) {
        fprintf(stderr, "Failed to load texture\n");
        return 0;
    }

    glBindVertexArray(VAO);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, imageFormat,
        GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    stbi_image_free(data);
    return texture;
}
void project_3d(struct shader const* s, vec3 position, bool rotate)
{
    // Time to go 3D, ya heard?
    mat4 model = GLM_MAT4_IDENTITY_INIT;
    glm_translate(model, position);
    if (rotate) {
        vec3 rotate_vector = { 0.5f, 1.0f, 0.0f };
        glm_rotate(model, (float)glfwGetTime() * glm_rad(50.0f), rotate_vector);
    }

    mat4 view = GLM_MAT4_IDENTITY_INIT;
    vec3 translate_vector = { 0.0f, 0.0f, -3.0f };
    glm_translate(view, translate_vector);

    mat4 projection;
    glm_perspective(glm_rad(45.0f), 800.0f / 600.0f, 0.1f, 100.0f, projection);

    int model_loc = glGetUniformLocation(s->ID, "model");
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, (float*)model);

    int view_loc = glGetUniformLocation(s->ID, "view");
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, (float*)view);

    int projection_loc = glGetUniformLocation(s->ID, "projection");
    glUniformMatrix4fv(projection_loc, 1, GL_FALSE, (float*)projection);
}

int main(void)
{
    GLFWwindow* window = setupWindow();
    if (window == NULL) {
        return 1;
    }

    // Cube
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
    };

    vec3 cube_positions[] = {
        { 0.0f, 0.0f, 0.0f },
        { 2.0f, 5.0f, -15.0f },
        { -1.5f, -2.2f, -2.5f },
        { -3.8f, -2.0f, -12.3f },
        { 2.4f, -0.4f, -3.5f },
        { -1.7f, 3.0f, -7.5f },
        { 1.3f, -2.0f, -2.5f },
        { 1.5f, 2.0f, -2.5f },
        { 1.5f, 0.2f, -1.5f },
        { -1.3f, 1.0f, -1.5f }
    };

    unsigned int VAO
        = createShape(vertices, sizeof(vertices));

    unsigned int texture1 = createTexture(VAO, "../src/container.jpg", GL_RGB);
    if (texture1 == 0) {
        glDeleteVertexArrays(1, &VAO);
        glfwTerminate();
        return 1;
    }

    unsigned int texture2 = createTexture(VAO, "../src/awesomeface.png",
        GL_RGBA);
    if (texture1 == 0) {
        glDeleteVertexArrays(1, &VAO);
        glfwTerminate();
        return 1;
    }

    struct shader s;
    shaderInit(&s, "../src/shader.vs", "../src/shader.fs");

    glBindVertexArray(VAO);
    glUseProgram(s.ID);
    glUniform1i(glGetUniformLocation(s.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(s.ID, "texture2"), 1);
    float sampleRatio = 0.5;
    glUniform1f(glGetUniformLocation(s.ID, "sampleRatio"), sampleRatio);

    glEnable(GL_DEPTH_TEST);

    // Render loop:
    while (!glfwWindowShouldClose(window)) {
        processInput(window, &sampleRatio, &s);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

        glBindVertexArray(VAO);
        for (int i = 0; i < NUM_CUBES; i++) {
            bool rotate = i % 3 == 0;
            project_3d(&s, cube_positions[i], rotate);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
