#include "Head/SpriteRenderer.h"

#include <GLCall.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

SpriteRenderer::SpriteRenderer(Shader& shader)
{
    this->shader = shader;
    this->initRenderData();
}

SpriteRenderer::~SpriteRenderer()
{
    GLCall(glDeleteVertexArrays(1, &this->quadVAO));
}

void SpriteRenderer::initRenderData()
{
    // Configure VAO/VBO
    GLuint VBO;
    GLfloat vertices[] = {
        // Pos      // Tex
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    GLCall(glGenVertexArrays(1, &this->quadVAO));
    GLCall(glGenBuffers(1, &VBO));

    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

    GLCall(glBindVertexArray(this->quadVAO));
    GLCall(glEnableVertexAttribArray(0));
    GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0));
    //½â°ó
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    GLCall(glBindVertexArray(0));
}

void SpriteRenderer::DrawSprite(Texture2D& texture, glm::vec2 position, glm::vec2 size, GLfloat rotate, glm::vec3 color)
{
    // Prepare transformations
    this->shader.Bind();
    glm::mat4 model;
    model = glm::translate(model, glm::vec3(position, 0.0f));  // First translate (transformations are: scale happens first, then rotation and then finall translation happens; reversed order)

    model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // Move origin of rotation to center of quad
    model = glm::rotate(model, rotate, glm::vec3(0.0f, 0.0f, 1.0f)); // Then rotate
    model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f)); // Move origin back

    model = glm::scale(model, glm::vec3(size, 1.0f)); // Last scale

    this->shader.SetUniformsMat4f("model", model);

    // Render textured quad
    this->shader.SetUniforms3f("spriteColor", color);

    GLCall(glActiveTexture(GL_TEXTURE0));
    texture.Bind();

    GLCall(glBindVertexArray(this->quadVAO));
    GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
    GLCall(glBindVertexArray(0));
}