#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
uniform float xOffset;

void main()
{
    float xPos = aPos.x + xOffset;

    gl_Position = vec4(xPos, -aPos.y, aPos.z, 1.0);
    ourColor = vec3(xPos, -aPos.y, aPos.z);
}
