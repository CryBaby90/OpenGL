#shader fragment
#version 330 core

out vec4 FragColor;

uniform sampler2D texture1;
uniform sampler2D texture2;
uniform float mixValue;
in vec2 TexCoord;

void main()
{
    FragColor = mix(texture(texture1, TexCoord), texture(texture2, vec2(1.0 - TexCoord.x, TexCoord.y)), mixValue);
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);
}
