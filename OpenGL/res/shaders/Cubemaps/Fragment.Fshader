#shader fragment
#version 330 core

/* 正常箱子
out vec4 FragColor;

uniform sampler2D texture1;
in vec2 TexCoord;

void main()
{
    FragColor = texture(texture1, TexCoord);
}*/

///反射
out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPos;
uniform samplerCube skybox;

void main()
{             
    vec3 I = normalize(Position - cameraPos);
    vec3 R = reflect(I, normalize(Normal));
    //折射 菲涅尔
    /*
    空气	1.00
    水	1.33
    冰	1.309
    玻璃	1.52
    钻石	2.42
    */
    float ratio = 2.42 / 1.52;
    R = refract(I, normalize(Normal), ratio);
    FragColor = vec4(texture(skybox, R).rgb, 1.0);
}
