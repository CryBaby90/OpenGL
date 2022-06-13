#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 in_Color;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    in_Color = aColor;
}

