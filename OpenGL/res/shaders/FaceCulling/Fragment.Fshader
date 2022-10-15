#shader fragment
#version 330 core

out vec4 FragColor;

uniform sampler2D texture1;
in vec2 TexCoord;

void main()
{
    vec4 texColor = texture(texture1, vec2(TexCoord.x, 1.0 - TexCoord.y));
    if(texColor.a < 0.1)
        discard;
    FragColor = texColor;
}
