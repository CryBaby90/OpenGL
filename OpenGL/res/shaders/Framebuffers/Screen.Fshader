#shader fragment
#version 330 core

out vec4 FragColor;

uniform sampler2D screenTexture;
in vec2 TexCoords;

const float offset = 1.0 / 300.0;  

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    FragColor = vec4(col, 1.0);

    //���� ��ɫȡ��
    FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);

    //�Ҷ�  ƽ����
    FragColor = texture(screenTexture, TexCoords);
    float average = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
    FragColor = vec4(average, average, average, 1.0);

    //�Ҷ� ��Ȩ
    FragColor = texture(screenTexture, TexCoords);
    average = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
    FragColor = vec4(average, average, average, 1.0);

    /*��Ч�� (Kernel)�����������(Convolution Matrix)����һ����������ֵ���飬��������Ϊ��ǰ�����أ�
      ���������ĺ�ֵ������Χ������ֵ�����������ӱ��һ��ֵ��
      ���ԣ��������������ڶԵ�ǰ������Χ��������������һ��С��ƫ�����������ݺ˽�����ϲ�
      ���������ҵ��Ĵ󲿷ֺ˽����е�Ȩ�ؼ�����֮��Ӧ�û����1��
      ������Ǽ�����������1�������ζ�����յ�������ɫ�����ԭ����ֵ�������߸����ˡ�
    */
    vec2 offsets[9] = vec2[](
        vec2(-offset,  offset), // ����
        vec2( 0.0f,    offset), // ����
        vec2( offset,  offset), // ����
        vec2(-offset,  0.0f),   // ��
        vec2( 0.0f,    0.0f),   // ��
        vec2( offset,  0.0f),   // ��
        vec2(-offset, -offset), // ����
        vec2( 0.0f,   -offset), // ����
        vec2( offset, -offset)  // ����
    );

    //��(Sharpen)�ˣ����������Χ���������أ���ÿ����ɫֵ
    /*float kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );*/

    //ģ����
    /*float kernel[9] = float[](
        1.0 / 16, 2.0 / 16, 1.0 / 16,
        2.0 / 16, 4.0 / 16, 2.0 / 16,
        1.0 / 16, 2.0 / 16, 1.0 / 16  
    );*/

    //��Ե����
    /*
    ע�⣬���ڶ���Ļ�����ı�Ե���в�����ʱ�����ڻ��������������Χ��8�����ؽ��в�������ʵ��ȡ������֮������ء�
    ���ڻ��Ʒ�ʽĬ����GL_REPEAT��������û�����õ������ȡ��������Ļ��һ�ߵ����أ�
    ����һ�ߵ����ر���Ӧ�ö��������ز���Ӱ�죬��Ϳ��ܻ�����Ļ��Ե��������ֵ����ơ�
    Ϊ��������һ���⣬���ǿ��Խ���Ļ�����Ļ��Ʒ�ʽ������ΪGL_CLAMP_TO_EDGE��
    ��������ȡ�������������ʱ�����ܹ��ظ���Ե������������ȷ�ع������յ�ֵ�ˡ�
    */
    float kernel[9] = float[](
        -1, -1, -1,
        -1,  8, -1,
        -1, -1, -1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }
    col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];

    FragColor = vec4(col, 1.0);
}