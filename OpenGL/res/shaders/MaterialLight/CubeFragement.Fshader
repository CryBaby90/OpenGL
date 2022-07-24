#shader fragment
#version 330 core

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

struct Material {
    vec3 ambient; //环境光
    vec3 diffuse; //漫反射
    vec3 specular;//高光
    float shininess; //反光度
}; 
uniform Material material;

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    //环境光
    vec3 ambient = material.ambient * light.ambient;

    //漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);//实际上是物体指向光源的方向
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = (diff * material.diffuse) * light.diffuse;
    
    //镜面高光
    vec3 viewDir = normalize(viewPos - FragPos);
    //vec3 reflectDir = reflect(-lightDir, norm);
    vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir; //2(n·l)n - l 反射方向计算
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = (spec * material.specular) * light.specular;
   
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);
}
