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
	//开启混合
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

    // 当面加载完成之后，我们需要定义字体大小，这表示着我们要从字体面中生成多大的字形
    FT_Set_Pixel_Sizes(face, 0, 48);

    // 禁用字节对齐限制
    //OpenGL要求所有的纹理都是4字节对齐的，即纹理的大小永远是4字节的倍数。
    //通常这并不会出现什么问题，因为大部分纹理的宽度都为4的倍数并/或每像素使用4个字节，
    //但是现在我们每个像素只用了一个字节，它可以是任意的宽度。通过将纹理解压对齐参数设为1，
    //这样才能确保不会有对齐问题（它可能会造成段错误）。
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // 遍历了ASCII集中的全部128个字符，并获取它们对应的字符字形
    for (unsigned char c = 0; c < 128; c++)
    {
        // 一个FreeType面中包含了一个字形的集合。我们可以调用FT_Load_Char函数来将其中一个字形设置为激活字形
        //通过将FT_LOAD_RENDER设为加载标记之一，我们告诉FreeType去创建一个8位的灰度位图，
        //我们可以通过face->glyph->bitmap来访问这个位图。
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        //属性	获取方式	生成位图描述
        //width	    face->glyph->bitmap.width	位图宽度（像素）
        //height	face->glyph->bitmap.rows	位图高度（像素）
        //bearingX	face->glyph->bitmap_left	水平距离，即位图相对于原点的水平位置（像素）
        //bearingY	face->glyph->bitmap_top	垂直距离，即位图相对于基准线的垂直位置（像素）
        //advance	face->glyph->advance.x	水平预留值，即原点到下一个字形原点的水平距离（单位：1 / 64像素）

        //我们这里将纹理的internalFormat和format设置为GL_RED
        //通过字形生成的位图是一个8位灰度图，它的每一个颜色都由一个字节来表示。
        //因此我们需要将位图缓冲的每一字节都作为纹理的颜色值。这是通过创建一个特殊的纹理实现的，
        //这个纹理的每一字节都对应着纹理颜色的红色分量（颜色向量的第一个字节）。
        //如果我们使用一个字节来表示纹理的颜色，我们需要注意OpenGL的一个限制：《字节对齐限制》
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
    //销毁 FreeType
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    //每个2D四边形需要6个顶点，每个顶点又是由一个4float向量（译注：一个纹理坐标和一个顶点坐标）组成
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

test::TestTextRendering::~TestTextRendering()
{
	//资源释放
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
    // 激活对应的渲染状态	
    m_Shader->Bind();
    m_Shader->SetUniforms3f("textColor", color);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(m_VAO);

    // 遍历文本中所有的字符
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // 对每个字符更新VBO
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // 在四边形上绘制字形纹理
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // 更新VBO内存的内容
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // 绘制四边形
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // 更新位置到下一个字形的原点，注意单位是1/64像素
        x += (ch.Advance >> 6) * scale; // 位偏移6个单位来获取单位为像素的值 (2^6 = 64)
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    m_Shader->Unbind();
}
