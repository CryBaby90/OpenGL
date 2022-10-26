#shader fragment
#version 330 core

/*
out vec4 FragColor;
in vec3 fColor;

void main()
{
    FragColor = vec4(fColor, 1.0);
}
*/

/*
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{
    FragColor = texture(texture_diffuse1, TexCoords);
}
*/

out vec4 FragColor;

void main()
{
    FragColor = vec4(1.0, 1.0, 0.0, 1.0);
}
