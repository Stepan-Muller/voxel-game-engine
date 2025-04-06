#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class Gui {
public:
	/**
	 * @brief Constructor for the Gui class.
	 *
	 * @param window Pointer to the GLFW window
	 */
	Gui(GLFWwindow* window);
	
	/**
	 * @brief Destructor for the Gui class, cleanup.
	 */
	~Gui();

	/**
	 * @brief Render the menu GUI.
	 */
	void render();

	/**
	 * @brief Parameters of the voxel selected for placement in the menu (R, G, B, A, reflectivity).
	 */
	float selectedVoxel[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	
	/**
	 * @brief Whether the voxel selected for placement in the menu is a collision voxel.
	 */
	bool selectedVoxelCollision = true;
};