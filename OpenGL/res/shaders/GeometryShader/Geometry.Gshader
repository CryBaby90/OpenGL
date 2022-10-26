#shader geometry
#version 330 core
/*
layout(points) in;
layout(points, max_vertices = 1) out;

void main() {
    gl_Position = gl_in[0].gl_Position + vec4(-0.45, 0.0, 0.0, 0.0);;
    EmitVertex();
    EndPrimitive();
}
*/

/*
layout(points) in;
layout(triangle_strip, max_vertices = 5) out;

// 我们并不是必须要用接口块来向几何着色器传递数据。如果顶点着色器发送的颜色向量是out vec3 vColor，我们也可以这样写：
// in vec3 vColor[];
// 然而，接口块在几何着色器这样的着色器中会更容易处理一点。实际上，几何着色器的输入能够变得非常大，
// 将它们合并为一个大的接口块数组会更符合逻辑一点。
in VS_OUT{
    vec3 color;
} gs_in[];

out vec3 fColor; //为下个片段着色器阶段声明一个输出颜色向量

void build_house(vec4 position)
{
    fColor = gs_in[0].color; // gs_in[0] 因为只有一个输入顶点
    gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左下
    EmitVertex();
    gl_Position = position + vec4(0.2, -0.2, 0.0, 0.0);    // 2:右下
    EmitVertex();
    gl_Position = position + vec4(-0.2, 0.2, 0.0, 0.0);    // 3:左上
    EmitVertex();
    gl_Position = position + vec4(0.2, 0.2, 0.0, 0.0);    // 4:右上
    EmitVertex();
    gl_Position = position + vec4(0.0, 0.4, 0.0, 0.0);    // 5:顶部
    fColor = vec3(1.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

// 你可以看到，有了几何着色器，你甚至可以将最简单的图元变得十分有创意。
// 因为这些形状是在GPU的超快硬件中动态生成的，这会比在顶点缓冲中手动定义图形要高效很多。
// 因此，几何缓冲对简单而且经常重复的形状来说是一个很好的优化工具，比如体素(Voxel)世界中的方块和室外草地的每一根草。
void main() {
    build_house(gl_in[0].gl_Position);
}
*/

/*
layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT{
    vec2 texCoords;
} gs_in[];

out vec2 TexCoords;

uniform float time;

//爆破效果
vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 0.0);
}

vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[2].gl_Position) - vec3(gl_in[1].gl_Position);
    return normalize(cross(a, b));
}

void main() {
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    TexCoords = gs_in[0].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    TexCoords = gs_in[1].texCoords;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    TexCoords = gs_in[2].texCoords;
    EmitVertex();
    EndPrimitive();
}
*/

layout(triangles) in;
layout(line_strip, max_vertices = 6) out;

in VS_OUT{
    vec3 normal;
} gs_in[];

const float MAGNITUDE = 0.002;

uniform mat4 proj;

void GenerateLine(int index)
{
    gl_Position = proj * gl_in[index].gl_Position;
    EmitVertex();
    gl_Position = proj * (gl_in[index].gl_Position +
        vec4(gs_in[index].normal, 0.0) * MAGNITUDE);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    GenerateLine(0); // 第一个顶点法线
    GenerateLine(1); // 第二个顶点法线
    GenerateLine(2); // 第三个顶点法线
}
