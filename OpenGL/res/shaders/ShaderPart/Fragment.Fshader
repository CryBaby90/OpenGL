#shader fragment
#version 330 core
out vec4 FragColor;

uniform vec4 ourColor; 
in vec3 in_Color;

void main()
{
    //FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    FragColor = ourColor;
    FragColor = vec4(in_Color, 1.0f);
}
