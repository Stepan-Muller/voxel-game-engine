#pragma once

#include "gui.h"

#include "player.h"
#include "sound.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class GameGui : public IGui {
public:
	GameGui(Sound* _sound);
	
	/**
	 * @brief Render the menu GUI.
	 */
	void render(Player* player);

private:
	Sound* sound;
	
	void renderMenu(Player* player);

	void renderInGame();
};