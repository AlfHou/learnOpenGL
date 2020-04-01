#version 330 core
in vec3 Normal;
in vec3 frag_position;

out vec4 frag_color;


uniform vec3 object_color;
uniform vec3 light_color;
uniform vec3 light_position;
uniform vec3 view_position;


void main()
{
    float ambient_strength = 0.1;
    vec3 ambient = ambient_strength * light_color;


    vec3 norm = normalize(Normal);
    vec3 light_dir = normalize(light_position - frag_position);

    float diff = max(dot(norm, light_dir), 0.0);
    vec3 diffuse = diff * light_color;

    float specular_strength = 0.5;

    vec3 view_dir = normalize(view_position - frag_position);
    vec3 reflect_dir = reflect(-light_dir, norm);

    float spec = pow(max(dot(view_dir, reflect_dir), 0.0), 32);
    vec3 specular = specular_strength * spec * light_color;

    vec3 result = (ambient + diffuse + specular) * object_color;
    frag_color = vec4(result, 1.0 );
}
