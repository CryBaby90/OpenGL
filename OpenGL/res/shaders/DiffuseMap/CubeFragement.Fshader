#shader fragment
#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

struct Material {
    vec3 ambient; //������
    sampler2D diffuse; //������ sampler2D ����ν�Ĳ�͸������(Opaque Type)��Ҳ����˵���ǲ��ܽ���ʵ������ֻ��ͨ��uniform��������
    sampler2D specular;//�߹�
    sampler2D emission;
    float shininess; //�����
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
    //������
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

    //������
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);//ʵ����������ָ���Դ�ķ���
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb; 
    
    //����߹�
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    //vec3 reflectDir = 2 * dot(norm, lightDir) * norm - lightDir; //2(n��l)n - l ���䷽�����
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;
    //vec3 specular = light.specular * spec * (vec3(1.0) - vec3(texture(material.specular, TexCoords))) //��ת�������ͼ����ɫֵ
   
    //�������ͼ
    vec3 emission = texture(material.emission, TexCoords).xyz; //�������ͼ ʹ��xyz
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}