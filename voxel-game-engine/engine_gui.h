#pragma once

#include "gui.h"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class EngineGui {
public:
	/**
	 * @brief Constructor for the Gui class.
	 *
	 * @param window Pointer to the GLFW window
	 */
	EngineGui(GLFWwindow* window, IGui* gameGui);

	/**
	 * @brief Destructor for the Gui class, cleanup.
	 */
	~EngineGui();

	/**
	 * @brief Render the menu GUI.
	 */
	void render(Player* player);

private:
	IGui* gameGui;
};