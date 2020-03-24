#include <glad/glad.h>
// Glad needs to be before GLFW
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cglm/cglm.h"

#include "camera.h"
#include "shader.h"

#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define NUM_CUBES 10

struct camera cam;

// Callback from GLFW that the window was resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    (void)window;
    glViewport(0, 0, width, height);
}

// Calback from GLFW that mouse has moved
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    static float last_x;
    static float last_y;

    // This will NOT be set back to true every time
    static bool first_mouse = true;

    if (first_mouse) {
        first_mouse = false;
        last_x = xpos;
        last_y = ypos;
    }
    (void)window;
    float xoffset = xpos - last_x;
    float yoffset = ypos - last_y;

    last_x = xpos;
    last_y = ypos;

    camera_process_mouse_movement(&cam, xoffset, yoffset);
}
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    (void)window;
    (void)xoffset;
    camera_process_scroll(&cam, yoffset);
}

void process_input(GLFWwindow* window, float* sample_ratio,
    struct shader* const s, struct camera* const cam, float delta_time)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        if (*sample_ratio < 1.0) {
            *sample_ratio = *sample_ratio + 0.01;
            shader_set_float(s, "sampleRatio", *sample_ratio);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        if (*sample_ratio > 0) {
            *sample_ratio = *sample_ratio - 0.01;
            shader_set_float(s, "sampleRatio", *sample_ratio);
        }
    }
    // Controll camera
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera_process_keyboard(cam, FORWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera_process_keyboard(cam, BACKWARD, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera_process_keyboard(cam, LEFT, delta_time);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera_process_keyboard(cam, RIGHT, delta_time);
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
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return NULL;
    }

    // Lock mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

    // Initialize camera
    camera_init(&cam);

    struct shader s;
    shader_init(&s, "../src/shader.vs", "../src/shader.fs");

    glBindVertexArray(VAO);
    glUseProgram(s.ID);
    glUniform1i(glGetUniformLocation(s.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(s.ID, "texture2"), 1);

    float sample_ratio = 0.5;
    shader_set_float(&s, "sampleRatio", sample_ratio);

    glEnable(GL_DEPTH_TEST);

    float delta_time = 0.0f;
    float last_frame = 0.0f;

    // Render loop:
    while (!glfwWindowShouldClose(window)) {
        process_input(window, &sample_ratio, &s, &cam, delta_time);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);

        // Pass projection matrix to shader
        mat4 projection;
        glm_perspective(glm_rad(cam.fov),
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, 0.1f, 100.0f,
            projection);
        shader_set_mat4(&s, "projection", projection);

        // Camera view transformation
        mat4 view;
        camera_get_view_matrix(&cam, view);
        shader_set_mat4(&s, "view", view);

        glBindVertexArray(VAO);
        for (int i = 0; i < NUM_CUBES; i++) {
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model, cube_positions[i]);
            vec3 rotate_vector = { 0.5f, 1.0f, 0.0f };
            glm_rotate(model, (float)glfwGetTime() * glm_rad(50.0f),
                rotate_vector);
            shader_set_mat4(&s, "model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
    }
    glDeleteVertexArrays(1, &VAO);
    glfwTerminate();
    return 0;
}
