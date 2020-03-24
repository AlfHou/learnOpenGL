#include <cglm/cglm.h>
#include <math.h>

#include "camera.h"

void camera_init(struct camera* cam)
{
    *cam = (struct camera) {
        .yaw = -90,
        .pitch = 0,
        .fov = 45,
        .movement_speed = 3.5f,
        .mouse_sensitivity = 0.05f,
        .camera_position = { 0.0f, 0.0f, 3.0f },
        .camera_front = { 0.0f, 0.0f, -1.0f },
        .camera_up = { 0.0f, 1.0f, 0.0f }
    };
}

void camera_process_mouse_movement(struct camera* const cam, float xoffset,
    float yoffset)
{
    xoffset *= cam->mouse_sensitivity;
    yoffset *= cam->mouse_sensitivity;

    cam->yaw += xoffset;
    cam->pitch -= yoffset;

    if (cam->pitch > 89.0f) {
        cam->pitch = 89.0f;
    }
    if (cam->pitch < -89.0f) {
        cam->pitch = -89.0f;
    }
}
void camera_process_scroll(struct camera* const cam, float yoffset)
{
    cam->fov -= yoffset;
    if (cam->fov > 1.0f && cam->fov < 45.f) {
        // Do nothing
    } else if (cam->fov <= 1.0f) {
        cam->fov = 1.1f;
    } else if (cam->fov >= 45.0f) {
        cam->fov = 44.9f;
    }
}
void camera_process_keyboard(struct camera* const cam,
    enum camera_movement direction, float delta_time)
{
    float velocity = cam->movement_speed * delta_time;
    switch (direction) {
    case FORWARD: {
        vec3 calculated_camera_front;
        glm_vec3_scale(cam->camera_front, velocity, calculated_camera_front);

        glm_vec3_add(cam->camera_position, calculated_camera_front,
            cam->camera_position);
        break;
    }
    case BACKWARD: {
        vec3 calculated_camera_front;
        glm_vec3_scale(cam->camera_front, velocity, calculated_camera_front);

        glm_vec3_sub(cam->camera_position, calculated_camera_front,
            cam->camera_position);
        break;
    }
    case LEFT: {
        // Get camera right
        vec3 camera_right;
        glm_vec3_cross(cam->camera_front, cam->camera_up, camera_right);
        glm_normalize(camera_right);

        vec3 calculated_camera_front;
        glm_vec3_scale(camera_right, velocity, calculated_camera_front);

        glm_vec3_sub(cam->camera_position, calculated_camera_front,
            cam->camera_position);
        break;
    }
    case RIGHT: {
        // Get camera right
        vec3 camera_right;
        glm_vec3_cross(cam->camera_front, cam->camera_up, camera_right);
        glm_normalize(camera_right);

        vec3 calculated_camera_front;
        glm_vec3_scale(camera_right, velocity, calculated_camera_front);

        glm_vec3_add(cam->camera_position, calculated_camera_front,
            cam->camera_position);
        break;
    }
    }
}
void camera_get_view_matrix(struct camera const* const cam, mat4 out)
{
    vec3 dir;
    dir[0] = cos(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
    dir[1] = sin(glm_rad(cam->pitch));
    dir[2] = sin(glm_rad(cam->yaw)) * cos(glm_rad(cam->pitch));
    glm_normalize(dir);

    glm_vec3_copy(dir, (float*)cam->camera_front);
    glm_vec3_add((float*)cam->camera_position, (float*)cam->camera_front, dir);

    glm_lookat((float*)cam->camera_position, dir, (float*)cam->camera_up, out);
}
