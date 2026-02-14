#pragma once

#include "gui.h"

#include "player.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class GameGui : public IGui {
public:
	/**
	 * @brief Render the menu GUI.
	 */
	void render(Player* player);

private:
	void renderMenu(Player* player);

	void renderInGame();
};