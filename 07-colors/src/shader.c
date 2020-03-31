#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>

#include "shader.h"

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
    char* buffer = malloc(sizeof(char) * stringSize + 1);

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

void shader_set_float(struct shader* const s, char* const uniformName, float value)
{
    glUniform1f(glGetUniformLocation(s->ID, uniformName), value);
}
void shader_set_mat4(struct shader* const s, char* const uniform_name, mat4 val)
{
    int uniform_loc = glGetUniformLocation(s->ID, uniform_name);
    glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, (float*)val);
}

void shader_set_vec3(struct shader* const s, char* const uniform_name, vec3 val)
{
    int uniform_loc = glGetUniformLocation(s->ID, uniform_name);
    glUniform3fv(uniform_loc, 1, (float*)val);
}

void shader_init(struct shader* instance, const char* vertex_path,
        const char* fragment_path)
{
    char* vertexShaderBuffer = readShader(vertex_path);
    if (vertexShaderBuffer == NULL) {
        return;
    }

    unsigned int vertexShaderId = compileShader(vertexShaderBuffer,
            GL_VERTEX_SHADER);
    free(vertexShaderBuffer);

    // Create fragment shader
    char* fragmentShaderBuffer = readShader(fragment_path);
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
