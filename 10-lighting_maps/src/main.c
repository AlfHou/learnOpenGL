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
#define NUM_CUBES 1

struct camera cam;

struct vao_and_vbo {
    unsigned int VAO;
    unsigned int VBO;
};

unsigned int load_texture(char const* path)
{
    stbi_set_flip_vertically_on_load(true);

    int width;
    int height;
    int nrChannels;
    unsigned char* data = stbi_load(path, &width, &height,
        &nrChannels, 0);
    if (data == NULL) {
        fprintf(stderr, "Failed to load texture\n");
        stbi_image_free(data);
        return 0;
    }
    GLenum format;
    if (nrChannels == 1) {
        format = GL_RED;
    }
    if (nrChannels == 3) {
        format = GL_RGB;
    }
    if (nrChannels == 4) {
        format = GL_RGBA;
    }

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format,
        GL_UNSIGNED_BYTE, data);

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);
    return texture;
}

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

void process_input(GLFWwindow* window, struct camera* const cam, float delta_time)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
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
struct vao_and_vbo create_shape(float* vertices, unsigned long sizeVertices)
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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
        (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    struct vao_and_vbo ret = {
        .VAO = VAO,
        .VBO = VBO
    };
    return ret;
}

unsigned int create_light(unsigned int VBO)
{
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // We only need to bind the VBO, the container's VBO's data already contains
    // the data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Set the vertex attributes (only position data for our lamp)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    return lightVAO;
}

int main(void)
{
    GLFWwindow* window = setupWindow();
    if (window == NULL) {
        return 1;
    }

    // Cube
    float vertices[] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

    vec3 cube_positions[] = {
        { 0.0f, 0.0f, 0.0f }
    };

    struct vao_and_vbo shape = create_shape(vertices, sizeof(vertices));
    unsigned int diffuse_map = load_texture("../src/container2.png");

    unsigned int specular_map = load_texture("../src/container2_specular.png");

    unsigned int lightVAO = create_light(shape.VBO);

    // Initialize camera
    camera_init(&cam);

    struct shader light_source_shader;
    shader_init(&light_source_shader, "../src/light_source_shader.vs",
        "../src/light_source_shader.fs");
    glBindVertexArray(lightVAO);
    glUseProgram(light_source_shader.ID);
    vec3 light_pos = { 1.2f, 1.0f, 2.0f };

    struct shader s;
    shader_init(&s, "../src/shader.vs", "../src/shader.fs");
    glBindVertexArray(shape.VAO);

    shader_set_int(&s, "material.diffuse", 0);
    vec3 object_specular_vector = { 0.5f, 0.5f, 0.5f };
    shader_set_vec3(&s, "material.specular", object_specular_vector);
    shader_set_float(&s, "material.shininess", 32.f);

    glUseProgram(s.ID);

    glEnable(GL_DEPTH_TEST);

    float delta_time = 0.0f;
    float last_frame = 0.0f;

    // Render loop:
    while (!glfwWindowShouldClose(window)) {
        process_input(window, &cam, delta_time);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(s.ID);

        // Set light color
        vec3 light_color = { 1.0f, 1.0f, 1.0f };

        vec3 diffuse_color;
        vec3 diffuse_factor = { 0.5f, 0.5f, 0.5f };
        glm_vec3_mul(light_color, diffuse_factor, diffuse_color);

        vec3 ambient_color;
        vec3 ambient_factor = { 0.1f, 0.1f, 0.1f };
        glm_vec3_mul(diffuse_color, ambient_factor, ambient_color);

        shader_set_vec3(&s, "light.ambient", ambient_color);

        shader_set_vec3(&s, "light.diffuse", diffuse_color);

        vec3 object_light_specular_vec = { 1.0f, 1.0f, 1.0f };
        shader_set_vec3(&s, "light.specular", object_light_specular_vec);

        shader_set_vec3(&s, "light.position", light_pos);

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

        glBindVertexArray(shape.VAO);
        for (int i = 0; i < NUM_CUBES; i++) {
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model, cube_positions[i]);
            vec3 rotate_vector = { 0.5f, 1.0f, 0.0f };
            glm_rotate(model, (float)glfwGetTime() * glm_rad(50.0f),
                rotate_vector);
            shader_set_mat4(&s, "model", model);
            shader_set_vec3(&s, "view_position", cam.camera_position);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        shader_set_int(&s, "material.specular", 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuse_map);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specular_map);

        // Render light
        glUseProgram(light_source_shader.ID);
        shader_set_mat4(&light_source_shader, "projection", projection);
        shader_set_mat4(&light_source_shader, "view", view);

        mat4 model = GLM_MAT4_IDENTITY_INIT;

        light_pos[0] = sin(glfwGetTime() * 1) * 5;
        light_pos[1] = sin(glfwGetTime() * 3) * 4;
        light_pos[2] = cos(glfwGetTime() * 1) * 5;
        glm_translate(model, light_pos);

        vec3 light_scale = { 0.2f, 0.2f, 0.2f };
        glm_scale(model, light_scale);
        shader_set_mat4(&light_source_shader, "model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();

        float current_frame = glfwGetTime();
        delta_time = current_frame - last_frame;
        last_frame = current_frame;
    }
    glDeleteVertexArrays(1, &shape.VAO);
    glfwTerminate();
    return 0;
}
