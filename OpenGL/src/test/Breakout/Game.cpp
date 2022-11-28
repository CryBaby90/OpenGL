#include "Head/Game.h"
#include "Global.h"
#include "Head/ResourceManager.h"

#include <glm/ext/matrix_clip_space.hpp>

test::Game::Game()
	:State(GameState::GAME_ACTIVE), Keys(), Width(SCR_WIDTH), Height(SCR_HEIGHT)
{
	//开启面剔除
	GLCall(glEnable(GL_CULL_FACE));
	//开启混合
	GLCall(glEnable(GL_BLEND));
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

	Init();
}

test::Game::~Game()
{

}

void test::Game::Init()
{
	// Load shaders
	//auto shader = ResourceManager::LoadShader("res/shaders/Breakout/Sprite/Sprite.Vshader", "res/shaders/Breakout/Sprite/Sprite.Fshader", "null", m_ShaderName);
	Shader shader("res/shaders/Breakout/Sprite/Sprite.Vshader", "res/shaders/Breakout/Sprite/Sprite.Fshader", "null");
	// Configure shaders
	glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(this->Width), static_cast<GLfloat>(this->Height), 0.0f, -1.0f, 1.0f);
	shader.Bind();
	shader.SetUniform1i("image", 0);
	shader.SetUniformsMat4f("projection", projection);
	// Load textures
	ResourceManager::LoadTexture("res/textures/awesomeface.png", GL_TRUE, "face");
	// Set render-specific controls
	Renderer = std::make_unique<SpriteRenderer>(shader);
}

void test::Game::OnUpdate(float deltaTime)
{

}

void test::Game::OnRender()
{
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	auto texture = ResourceManager::GetTexture("face");
	Renderer->DrawSprite(texture, glm::vec2(200, 200), glm::vec2(300, 400), 45.0f, glm::vec3(0.0f, 1.0f, 0.0f));
}

void test::Game::OnImGuiRender()
{

}

void test::Game::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{

}

void test::Game::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{

}
