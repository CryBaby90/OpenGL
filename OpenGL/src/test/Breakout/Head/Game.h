#pragma once
#include "../../Test.h"
#include "../Head/SpriteRenderer.h"

enum GameState {
    GAME_ACTIVE,
    GAME_MENU,
    GAME_WIN
};

namespace test
{
    // Game holds all game-related state and functionality.
// Combines all game-related data into a single class for
// easy access to each of the components and manageability.
    class Game : public Test
    {
    public:
        // Game state
        GameState              State;
        GLboolean              Keys[1024];
        GLuint                 Width, Height;
        // Constructor/Destructor
        Game();
        ~Game();
        // Initialize game state (load all shaders/textures/levels)
        void Init();
        // GameLoop
        void OnUpdate(float deltaTime) override;
        void OnRender() override;
        void OnImGuiRender() override;
        void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
        void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

    private:
        std::string m_ShaderName = "sprite";
        std::unique_ptr<SpriteRenderer> Renderer;
    };
}
