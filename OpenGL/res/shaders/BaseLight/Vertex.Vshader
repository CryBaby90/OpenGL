#shader vertex
#version 330 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightingColor; // gouraud shading result

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normal;

//for gouraud shading result
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    gl_Position = proj * view * model * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
    //FragPos = vec3(model * vec4(aPos, 1.0)); //世界空间
    FragPos = vec3(view * model * vec4(aPos, 1.0)); //观察空间
    //Normal = mat3(transpose(inverse(model))) * aNormal; //法线矩阵： 模型矩阵左上角3x3部分的逆矩阵的转置矩阵
                                                        //注意，大部分的资源都会将法线矩阵定义为应用到模型-观察矩阵(Model-view Matrix)上的操作，
                                                        //但是由于我们只在世界空间中进行操作（不是在观察空间），我们只使用模型矩阵。
                                                        //矩阵求逆是一项对于着色器开销很大的运算,最好先在CPU上计算出法线矩阵
    Normal = normal * aNormal;

    // gouraud shading
    // ------------------------
    vec3 Position = vec3(model * vec4(aPos, 1.0));
    vec3 Normal = mat3(transpose(inverse(model))) * aNormal;

    // ambient
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - Position);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    // specular
    float specularStrength = 1.0; // this is set higher to better show the effect of Gouraud shading 
    vec3 viewDir = normalize(viewPos - Position);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    LightingColor = ambient + diffuse + specular;
}
