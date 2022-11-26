#include "TestTextRendering.h"
#include "Global.h"

#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

test::TestTextRendering::TestTextRendering()
	:m_Shader(nullptr)
{
	// OpenGL state
	// ------------
	GLCall(glEnable(GL_CULL_FACE));
	//�������
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	m_Shader = new Shader("res/shaders/TextRendering/Vertex.Vshader", "res/shaders/TextRendering/Fragment.Fshader");
	m_Shader->Bind();
	glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
	m_Shader->SetUniformsMat4f("projection", projection);
	m_Shader->Unbind();

	// FreeType
	// --------
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, "res/Fonts/arial.ttf", 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // ����������֮��������Ҫ���������С�����ʾ������Ҫ�������������ɶ�������
    FT_Set_Pixel_Sizes(face, 0, 48);

    // �����ֽڶ�������
    //OpenGLҪ�����е�������4�ֽڶ���ģ�������Ĵ�С��Զ��4�ֽڵı�����
    //ͨ���Ⲣ�������ʲô���⣬��Ϊ�󲿷�����Ŀ�ȶ�Ϊ4�ı�����/��ÿ����ʹ��4���ֽڣ�
    //������������ÿ������ֻ����һ���ֽڣ�������������Ŀ�ȡ�ͨ���������ѹ���������Ϊ1��
    //��������ȷ�������ж������⣨�����ܻ���ɶδ��󣩡�
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // ������ASCII���е�ȫ��128���ַ�������ȡ���Ƕ�Ӧ���ַ�����
    for (unsigned char c = 0; c < 128; c++)
    {
        // һ��FreeType���а�����һ�����εļ��ϡ����ǿ��Ե���FT_Load_Char������������һ����������Ϊ��������
        //ͨ����FT_LOAD_RENDER��Ϊ���ر��֮һ�����Ǹ���FreeTypeȥ����һ��8λ�ĻҶ�λͼ��
        //���ǿ���ͨ��face->glyph->bitmap���������λͼ��
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        //����	��ȡ��ʽ	����λͼ����
        //width	    face->glyph->bitmap.width	λͼ��ȣ����أ�
        //height	face->glyph->bitmap.rows	λͼ�߶ȣ����أ�
        //bearingX	face->glyph->bitmap_left	ˮƽ���룬��λͼ�����ԭ���ˮƽλ�ã����أ�
        //bearingY	face->glyph->bitmap_top	��ֱ���룬��λͼ����ڻ�׼�ߵĴ�ֱλ�ã����أ�
        //advance	face->glyph->advance.x	ˮƽԤ��ֵ����ԭ�㵽��һ������ԭ���ˮƽ���루��λ��1 / 64���أ�

        //�������ｫ�����internalFormat��format����ΪGL_RED
        //ͨ���������ɵ�λͼ��һ��8λ�Ҷ�ͼ������ÿһ����ɫ����һ���ֽ�����ʾ��
        //���������Ҫ��λͼ�����ÿһ�ֽڶ���Ϊ�������ɫֵ������ͨ������һ�����������ʵ�ֵģ�
        //��������ÿһ�ֽڶ���Ӧ��������ɫ�ĺ�ɫ��������ɫ�����ĵ�һ���ֽڣ���
        //�������ʹ��һ���ֽ�����ʾ�������ɫ��������Ҫע��OpenGL��һ�����ƣ����ֽڶ������ơ�
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            static_cast<unsigned int>(face->glyph->advance.x)
        };
        Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    //���� FreeType
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    //ÿ��2D�ı�����Ҫ6�����㣬ÿ������������һ��4float��������ע��һ�����������һ���������꣩���
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

test::TestTextRendering::~TestTextRendering()
{
	//��Դ�ͷ�
	delete m_Shader;
	GLCall(glDeleteVertexArrays(1, &m_VAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
}

void test::TestTextRendering::OnUpdate(float deltaTime)
{
}

void test::TestTextRendering::OnRender()
{
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderText(*m_Shader, "This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    RenderText(*m_Shader, "(C) LearnOpenGL.com", 540.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
}

void test::TestTextRendering::OnImGuiRender()
{
}

//// render line of text
// -------------------
void test::TestTextRendering::RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // �����Ӧ����Ⱦ״̬	
    m_Shader->Bind();
    m_Shader->SetUniforms3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    // �����ı������е��ַ�
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // ��ÿ���ַ�����VBO
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // ���ı����ϻ�����������
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // ����VBO�ڴ������
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // �����ı���
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // ����λ�õ���һ�����ε�ԭ�㣬ע�ⵥλ��1/64����
        x += (ch.Advance >> 6) * scale; // λƫ��6����λ����ȡ��λΪ���ص�ֵ (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_Shader->Unbind();
}
