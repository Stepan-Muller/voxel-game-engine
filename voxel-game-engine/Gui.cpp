#include "Gui.h"

/**
 * @brief Constructor for the Gui class.
 *
 * @param window Pointer to the GLFW window
 */
Gui::Gui(GLFWwindow* window)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();  // darkmode

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");  // match shader version
}

/**
 * @brief Destructor for the Gui class, cleanup.
 */
Gui::~Gui()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

/**
 * @brief Render the menu GUI.
 */
void Gui::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Editor voxelù");

    ImGui::ColorPicker4("Barva voxelu", selectedVoxel);
    ImGui::SliderFloat("Odrazivost voxelu", &selectedVoxel[4], 0.0f, 1.0f);
    ImGui::Checkbox("Kolize", &selectedVoxelCollision);

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}