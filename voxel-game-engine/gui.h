#pragma once

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

class Gui {
public:
    Gui(GLFWwindow* window);
	~Gui();

	void render();

	float selectedVoxel[5] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f };
	bool selectedVoxelCollision = true;
};