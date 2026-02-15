#pragma once

#include "gui.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class EngineGui
{
public:
	/**
	 * @brief Constructor for the Gui class.
	 *
	 * @param window Pointer to the GLFW window
	 * @param gameGui Pointer to the game GUI, used for rendering the menu GUI and passing it to the engine GUI.
	 */
	EngineGui(GLFWwindow *window, IGui *gameGui);

	/**
	 * @brief Destructor for the Gui class, cleanup.
	 */
	~EngineGui();

	/**
	 * @brief Render the menu GUI.
	 * 
	 * @param player Pointer to the player, used for rendering the menu GUI and passing it to the game GUI.
	 */
	void render(Player *player);

private:
	/**
	 * @brief Pointer to the GLFW window.
	 */
	IGui *gameGui;
};