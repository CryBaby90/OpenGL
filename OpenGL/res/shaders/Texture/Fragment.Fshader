#shader fragment
#version 330 core

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
in vec2 TexCoord;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    //FragColor = texture(texture1, TexCoord);
}
