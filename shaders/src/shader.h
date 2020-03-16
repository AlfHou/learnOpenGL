#ifndef SHADER_H
#define SHADER_H
#include <glad/glad.h>

struct shader {
    unsigned int ID;
};
/* Creates a shader program using the GLSL source code from 'vertexPath'
 * and 'fragmentPath'
 */
void shaderInit(struct shader* instance, const char* vertexPath,
    const char* fragmentPath);

#endif
