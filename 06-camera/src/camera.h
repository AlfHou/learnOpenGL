#ifndef CAMERA_H
#define CAMERA_H
#include <cglm/cglm.h>
struct camera {
    vec3 camera_position;
    vec3 camera_front;
    vec3 camera_up;
    float movement_speed;
    float yaw;
    float pitch;
    float fov;
    float mouse_sensitivity;
};

// Camera moves
enum camera_movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

void camera_init(struct camera* cam);
void camera_process_mouse_movement(struct camera* const cam, float xoffset,
    float yoffset);
void camera_process_scroll(struct camera* const cam, float yoffset);
void camera_get_view_matrix(struct camera const* const cam, mat4 out);
void camera_process_keyboard(struct camera* const cam,
    enum camera_movement direction, float delta_time);
#endif
