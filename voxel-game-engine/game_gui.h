#pragma once

#include "gui.h"

#include "player.h"
#include "sound.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class GameGui : public IGui
{
public:
	/**
	 * @brief Constructor for the GameGui class.
	 */
	GameGui(Sound *_sound);

	/**
	 * @brief Render the menu GUI.
	 */
	void render(Player *player);

private:
	/**
	 * @brief Sound system.
	 */
	Sound *sound;

	/**
	 * @brief Render the menu GUI.
	 */
	void renderMenu(Player *player);

	/**
	 * @brief Render the in-game GUI.
	 */
	void renderInGame();
};