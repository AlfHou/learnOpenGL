#include <glad/glad.h>
// Glad needs to be before GLFW
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

// Vertex shader in GLSL
const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main()\n"
                                 "{\n"
                                 "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                 "}\0";

// Fragment shader in GLSL
const char* firstFragmentShaderSource = "#version 330 core\n"
                                        "out vec4 FragColor;\n"
                                        "void main()\n"
                                        "{\n"
                                        "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                        "}\0";

const char* secondFragmentShaderSource = "#version 330 core\n"
                                         "out vec4 FragColor;\n"
                                         "void main()\n"
                                         "{\n"
                                         "    FragColor = vec4(1.0f, 1.0f, 0.1f, 1.0f);\n"
                                         "}\0";

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    return VAO;
}

unsigned int createVertexShader()
{
    // Get vertex shader ID
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Add our shader source code to the vertex shader and compile
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Check compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("Failed to compile vertex shader: %s\n", infoLog);
    }
    return vertexShader;
}
unsigned int createFragmentShader(const char* fragmentShaderSource)
{
    // Get fragment shader id
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    int success;
    char infoLog[512];
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("Failed to compile fragment shader: %s\n", infoLog);
    }
    return fragmentShader;
}
unsigned int createShaderProgram(unsigned int vertexShader,
    unsigned int fragmentShader)
{
    // Link shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // Check link status
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Failed to link shader program: %s\n", infoLog);
    }

    // Delete shaders (they are now linked into the shader program)
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

int main(void)
{
    GLFWwindow* window = setupWindow();
    if (window == NULL) {
        return -1;
    }

    // Set up vertices for first triangle
    float verticesFirstTriangle[] = {
        -1.0f, -1.0f, 0.0f, // Lower left
        0.0f, -1.0f, 0.0f,  // Lower right
        -0.5f, 0.0f, 0.0f,  // Top
    };
    float verticesSecondTriangle[] = {
        0.0f, -1.0f, 0.0f, // Lower left
        1.0f, -1.0f, 0.0f, // Lower right
        0.5f, 0.0f, 0.0f   // Top
    };
    float verticesThirdTriangle[] = {
        -0.5f, 0.0f, 0.0f, // Lower left
        0.5f, 0.0f, 0.0f,  // Lower right
        0.0f, 1.0f, 0.0f   // Top
    };

    unsigned int firstVAO
        = createTriangle(verticesFirstTriangle, sizeof(verticesFirstTriangle));

    unsigned int secondVAO = createTriangle(verticesSecondTriangle,
        sizeof(verticesSecondTriangle));

    unsigned int thirdVAO = createTriangle(verticesThirdTriangle,
        sizeof(verticesThirdTriangle));

    unsigned int vertexShader = createVertexShader();

    unsigned int firstFragmentShader = createFragmentShader(firstFragmentShaderSource);

    unsigned int firstShaderProgram = createShaderProgram(vertexShader,
        firstFragmentShader);

    unsigned int secondFragmentShader = createFragmentShader(secondFragmentShaderSource);
    unsigned int secondShaderProgram = createShaderProgram(vertexShader,
        secondFragmentShader);

    glDeleteShader(vertexShader);

    // Render loop:
    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(firstShaderProgram);

        glBindVertexArray(firstVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(secondVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(secondShaderProgram);

        glBindVertexArray(thirdVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}
