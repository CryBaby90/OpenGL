#shader fragment
#version 330 core

out vec4 FragColor;

uniform sampler2D texture1;
in VS_OUT//接口块
{
    vec2 TexCoords;
} fs_in;


void main()
{
    FragColor = texture(texture1, fs_in.TexCoords);
    /*
    if(gl_FragCoord.x < 400)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);   

    if(gl_FrontFacing)
        FragColor = vec4(1.0, 0.0, 0.0, 1.0);
    else
        FragColor = vec4(0.0, 1.0, 0.0, 1.0);
    */
}

/*
#version 420 core // 注意GLSL的版本！
out vec4 FragColor;
    //any	默认值。提前深度测试是禁用的，你会损失很多性能
    //greater	你只能让深度值比gl_FragCoord.z更大
    //less	你只能让深度值比gl_FragCoord.z更小
    //unchanged	如果你要写入gl_FragDepth，你将只能写入gl_FragCoord.z的值
    //由我们自己设置深度值有一个很大的缺点，只要我们在片段着色器中对gl_FragDepth进行写入，
    //OpenGL就会（像深度测试小节中讨论的那样）禁用所有的提前深度测试(Early Depth Testing)。
    //它被禁用的原因是，OpenGL无法在片段着色器运行之前得知片段将拥有的深度值，因为片段着色器可能会完全修改这个深度值。
    //注意这个特性只在OpenGL 4.2版本或以上才提供

layout (depth_greater) out float gl_FragDepth;

void main()
{             
    FragColor = vec4(1.0);
    gl_FragDepth = gl_FragCoord.z + 0.1;
}*/

