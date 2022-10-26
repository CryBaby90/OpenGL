#shader vertex
#version 330 core
/*
layout(location = 0) in vec2 aPos;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
}
*/

/*
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

out VS_OUT{
    vec3 color;
} vs_out;

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, 0.0, 1.0);
    vs_out.color = aColor;
}
*/

/*
layout(location = 0) in vec3 aPos;
layout(location = 2) in vec2 aTexCoords;

out VS_OUT{
    vec2 texCoords;
} vs_out;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

void main()
{
    vs_out.texCoords = aTexCoords;
    gl_Position = proj * view * model * vec4(aPos, 1.0);
}
*/

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out VS_OUT{
    vec3 normal;
} vs_out;

uniform mat4 view;
uniform mat4 model;

//为了适配（观察和模型矩阵的）缩放和旋转，我们在将法线变换到观察空间坐标之前，
//先使用法线矩阵变换一次（几何着色器接受的位置向量是观察空间坐标，所以我们应该将法向量变换到相同的空间中）
void main()
{
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = vec3(vec4(normalMatrix * aNormal, 0.0));
    gl_Position = view * model * vec4(aPos, 1.0);
}
