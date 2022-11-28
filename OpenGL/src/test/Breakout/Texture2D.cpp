#include "Head/Texture2D.h"

Texture2D::Texture2D()
	: Width(0), Height(0), Internal_Format(GL_RGB), 
	Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), 
	Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
	GLCall(glGenTextures(1, &this->ID));
}

void Texture2D::Generate(GLuint width, GLuint height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;
    // ´´½¨ Texture
    GLCall(glBindTexture(GL_TEXTURE_2D, this->ID));
    GLCall(glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data));
    // ÉèÖÃ wrap and filter
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min));
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max));
    // Unbind texture
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::Bind() const
{
    GLCall(glBindTexture(GL_TEXTURE_2D, this->ID));
}
