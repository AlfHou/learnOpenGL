#include "shader.h"
#include <stdio.h>
#include <stdlib.h>

/* Read shader from file. Returns char* to string version of
 * the shader. Needs to be freed when no longer in use. Returns null on error.
 */
char* readShader(const char* path)
{
    FILE* file;
    file = fopen(path, "r");
    if (file == NULL) {
        fprintf(stderr, "shader.c, %s:", path);
        perror(NULL);
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    int stringSize = ftell(file);
    if (stringSize == -1) {
        fprintf(stderr, "shader.c, %s:", path);
        perror("");
        return NULL;
    }
    char* buffer = malloc(sizeof(char) * stringSize);

    rewind(file);

    int readSize = fread(buffer, sizeof(char), stringSize,
        file);

    buffer[stringSize] = '\0';

    if (readSize != stringSize) {
        printf("Failed to read shader\n");
        free(buffer);
        fclose(file);
        return NULL;
    }
    fclose(file);
    return buffer;
}

unsigned int compileShader(const char* shader, const GLenum shaderType)
{
    unsigned int shaderId;
    shaderId = glCreateShader(shaderType);

    // Add our shader source code to the vertex shader and compile
    glShaderSource(shaderId, 1, &shader, NULL);
    glCompileShader(shaderId);

    // Check compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shaderId, 512, NULL, infoLog);
        printf("Failed to compile shader: %s\n", infoLog);
    }
    return shaderId;
}
unsigned int createShaderProgram(unsigned int vertexShaderId,
    unsigned int fragmentShaderId)
{
    // Link shader program
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();

    glAttachShader(shaderProgram, vertexShaderId);
    glAttachShader(shaderProgram, fragmentShaderId);
    glLinkProgram(shaderProgram);

    // Check link status
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        printf("Failed to link shader program: %s\n", infoLog);
    }
    return shaderProgram;
}

void setFloat(struct shader* const s, char* const uniformName, float value)
{
    glUniform1f(glGetUniformLocation(s->ID, uniformName), value);
}

void shaderInit(struct shader* instance, const char* vertexPath,
    const char* fragmentPath)
{
    char* vertexShaderBuffer = readShader(vertexPath);
    if (vertexShaderBuffer == NULL) {
        return;
    }

    unsigned int vertexShaderId = compileShader(vertexShaderBuffer,
        GL_VERTEX_SHADER);
    free(vertexShaderBuffer);

    // Create fragment shader
    char* fragmentShaderBuffer = readShader(fragmentPath);
    if (fragmentShaderBuffer == NULL) {
        glDeleteShader(vertexShaderId);
        return;
    }

    unsigned int fragmentShaderId = compileShader(fragmentShaderBuffer,
        GL_FRAGMENT_SHADER);
    free(fragmentShaderBuffer);

    unsigned int shaderProgramId = createShaderProgram(vertexShaderId,
        fragmentShaderId);

    instance->ID = shaderProgramId;

    glDeleteShader(vertexShaderId);
    glDeleteShader(fragmentShaderId);
}
