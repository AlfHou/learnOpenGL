#version 330 core
in vec3 Normal;
in vec3 frag_position;

out vec4 frag_color;


struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;

uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;


void main()
{
    // Ambient
    vec3 ambient = light_color * material.ambient;


    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(light_position - frag_position);
    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = light_color * (diff * material.diffuse);

    // Specular
    vec3 view_dir = normalize(view_position - frag_position);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), material.shininess);
    vec3 specular = light_color * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    frag_color = vec4(result, 1.0);
}
