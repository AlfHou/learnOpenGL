#ifndef SHADER_H
#define SHADER_H
#include <cglm/cglm.h>
#include <glad/glad.h>

struct shader {
    unsigned int ID;
};
/* Creates a shader program using the GLSL source code from 'vertexPath'
 * and 'fragmentPath'
 */
void shader_init(struct shader* instance, const char* vertex_path,
        const char* fragment_path);

/* Set uniform 'uniformName' in shader 's' to value 'value'
*/
void shader_set_float(struct shader* const s, char* const uniformName, float value);

void shader_set_mat4(struct shader* const s, char* const uniform_name, mat4 val);

void shader_set_vec3(struct shader* const s, char* const uniform_name, vec3 val);

#endif
