#shader fragment
#version 330 core

out vec4 FragColor;

uniform sampler2D screenTexture;
in vec2 TexCoord;

void main()
{
    vec3 col = texture(screenTexture, TexCoord).rgb;
    FragColor = vec4(col, 1.0);
}
