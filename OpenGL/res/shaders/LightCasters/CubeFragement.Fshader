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
    float shininess; //反光度
}; 
uniform Material material;

struct Light {
    vec3 direction;//平行光使用
    vec3 position; //点光源使用

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff; //聚光灯使用 内圆锥
    float outerCutOff;// 外圆锥
};
uniform Light light;

uniform vec3 viewPos;

void main()
{
    vec3 lightDir = normalize(light.position - FragPos); //非平行光使用

    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon   = light.cutOff - light.outerCutOff; 
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   // 内外锥余弦差

    /*
    if(theta > light.cutOff) //无外圆锥（outerCutOff） 点乘结果是余弦值  0度 = 1.0f  90度 = 0.0f  
    {       
        // 执行光照计算
    }
    else  // 否则，使用环境光，让场景在聚光之外时不至于完全黑暗
        color = vec4(light.ambient * vec3(texture(material.diffuse, TexCoords)), 1.0);
    */

    //环境光
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    //漫反射
    vec3 norm = normalize(Normal);
    //vec3 lightDir = normalize(-light.direction); //平行光使用
    
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb; 
    
    //镜面高光
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    //vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir; //2(n·l)n - l 反射方向计算
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    //点光源 衰减
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                light.quadratic * (distance * distance)); //1.0 / Kc + Kl *d +Kq * d^2 
                                                          //常数项（通常1.0f, 保证式子不为负值）+ 一次项 * 距离 + 二次项 * 距离的平方
    //ambient  *= attenuation; 聚光灯不衰减环境光
    /*diffuse  *= attenuation;
    specular *= attenuation;*/

    //有外圆锥的聚光灯用
    diffuse  *= intensity;
    specular *= intensity;
   
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
