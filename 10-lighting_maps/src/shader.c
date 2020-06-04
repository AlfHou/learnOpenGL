#include <cglm/cglm.h>
#include <stdio.h>
#include <stdlib.h>

#include "shader.h"

/* Read shader from file. Returns char* to string version of
 * the shader. Needs to be freed when no longer in use. Returns null on error.
 */
char* read_shader(const char* path)
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

unsigned int compile_shader(const char* shader, const GLenum shaderType)
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

void shader_set_int(struct shader* const s, char* const uniformName, int value)
{
    glUniform1i(glGetUniformLocation(s->ID, uniformName), value);
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

void shader_init(struct shader* instance, char const* vertex_path,
    char const* fragment_path)
{

    char* vertex_shader_buffer = read_shader(vertex_path);
    if (vertex_shader_buffer == NULL) {
        return;
    }

    unsigned int vertex_shader_id = compile_shader(vertex_shader_buffer,
        GL_VERTEX_SHADER);
    free(vertex_shader_buffer);

    // Create fragment shader
    char* fragment_shader_buffer = read_shader(fragment_path);
    if (fragment_shader_buffer == NULL) {
        glDeleteShader(vertex_shader_id);
        return;
    }

    unsigned int fragment_shader_id = compile_shader(fragment_shader_buffer,
        GL_FRAGMENT_SHADER);
    free(fragment_shader_buffer);

    unsigned int shaderProgramId = createShaderProgram(vertex_shader_id,
        fragment_shader_id);

    instance->ID = shaderProgramId;

    glDeleteShader(vertex_shader_id);
    glDeleteShader(fragment_shader_id);
}
