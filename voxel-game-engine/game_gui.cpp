#include "game_gui.h"

void GameGui::render(Player* player) {   
	if (player->menu)
		renderMenu(player);
	else
		renderInGame();
}

void GameGui::renderMenu(Player* player)
{
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_Always);
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
        player->toggleMenu();

    ImGui::Spacing();

    if (ImGui::Button("Save Game", ImVec2(-1, 40)))
		player->saveGame();

    ImGui::Spacing();

    if (ImGui::Button("Load Game", ImVec2(-1, 40)))
        player->loadGame();

    ImGui::Spacing();

    if (ImGui::Button("Respawn", ImVec2(-1, 40)))
		player->respawn();

    ImGui::End();
}

void GameGui::renderInGame()
{

}