#shader fragment
#version 330 core

out vec4 FragColor;

uniform samplerCube skybox;
in vec3 TexCoord; //”√ vec3

void main()
{
    FragColor = texture(skybox, TexCoord);
}
