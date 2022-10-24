#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;

//std140布局
//shared布局 ： 使用共享布局时，GLSL是可以为了优化而对uniform变量的位置进行变动的，只要变量的顺序保持不变
//packed： 当使用紧凑(Packed)布局时，是不能保证这个布局在每个程序中保持不变的（即非共享），
//因为它允许编译器去将uniform变量从Uniform块中优化掉，这在每个着色器中都可能是不同的
layout(std140) uniform Matrices
{
    mat4 proj;
};
uniform mat4 view;
out vec3 TexCoord; //用 vec3

void main()
{
    TexCoord = aPos;
    vec4 pos = proj* view * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}

