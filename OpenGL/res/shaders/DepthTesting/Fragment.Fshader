#shader fragment
#version 330 core

out vec4 FragColor;

uniform sampler2D texture1;
in vec2 TexCoord;

float near = 0.1; 
float far  = 100.0; 

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));    
}

void main()
{
    //FragColor = texture(texture1, TexCoord);

    //内建gl_FragCoord向量的z值包含了那个特定片段的深度值
    FragColor = vec4(vec3(gl_FragCoord.z), 1.0);

    float depth = LinearizeDepth(gl_FragCoord.z) / far;
    FragColor = vec4(vec3(depth), 1.0);
}
