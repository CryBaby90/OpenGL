#shader fragment
#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    vec3 ambient; //环境光
    sampler2D diffuse; //漫反射 sampler2D 是所谓的不透明类型(Opaque Type)，也就是说我们不能将它实例化，只能通过uniform来定义它
    sampler2D specular;//高光
    sampler2D emission;
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
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    //漫反射
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);//实际上是物体指向光源的方向
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb; 
    
    //镜面高光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    //vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir; //2(n·l)n - l 反射方向计算
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    //vec3 specular = light.specular * spec * (vec3(1.0) - vec3(texture(material.specular, TexCoords))) //反转镜面光贴图的颜色值
   
    //放射光贴图
    vec3 emission = texture(material.emission, TexCoords).xyz; //放射光贴图 使用xyz
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}
