#include "game_gui.h"

GameGui::GameGui(Sound* _sound)
{
	sound = _sound;
}

void GameGui::render(Player* player) {   
	if (player->menu)
		renderMenu(player);
	else
		renderInGame();
}

void GameGui::renderMenu(Player* player)
{
    ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Always);
    ImGui::SetNextWindowPos(
        ImVec2(ImGui::GetIO().DisplaySize.x * 0.5f, ImGui::GetIO().DisplaySize.y * 0.5f),
        ImGuiCond_Always,
        ImVec2(0.5f, 0.5f)
    );

    ImGui::Begin("Pause Menu", nullptr,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove);

    if (ImGui::Button("Return to Game", ImVec2(-1, 40)))
    {
        sound->playSound("sounds/block.wav");

        player->toggleMenu();
    }

    ImGui::Spacing();

    if (ImGui::Button("Save Game", ImVec2(-1, 40)))
    {
        sound->playSound("sounds/block.wav");
        
        player->saveGame();
    }

    ImGui::Spacing();

    if (ImGui::Button("Load Game", ImVec2(-1, 40)))
    {
        sound->playSound("sounds/block.wav");
        
        player->loadGame();
    }

    ImGui::Spacing();

    if (ImGui::Button("Respawn", ImVec2(-1, 40)))
    {
        sound->playSound("sounds/block.wav");
        
        player->respawn();
    }

    ImGui::Spacing();

    if (ImGui::Button("Exit", ImVec2(-1, 40)))
    {
        sound->playSound("sounds/block.wav");

        player->exit();
    }

    ImGui::End();
}

void GameGui::renderInGame()
{

}