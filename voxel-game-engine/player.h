#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

#include "util.c"
#include "map.h"
#include "file_io.h"
#include "sound.h"
#include "voxel_interactor.h"
#include "engine_gui.h"

/* Verze OpenGL (4.6) */
#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 6

/* Fyzika */
#define GRAVITY 98.0f

/* Zvuk */
#define STEP_COOLDOWN 0.5f

class Player {
public:
    /**
     * @brief Constructor for the Player class.
     *
     * @param _map Pointer to the map object in which the player is located
     * @param _voxelInteractor Pointer to the voxel interactor, used for interacting with the world when placing and destroying blocks.
     * @param _gameGui Pointer to the game GUI, used for rendering the menu GUI and passing it to the engine GUI.
     * @param _sound Pointer to the sound system, used for playing sounds in the game.
     */
    Player(Map* _map, IVoxelInteractor* _voxelInteractor, IGui* _gameGui, Sound* _sound);

    /**
	 * @brief Toggle the visibility of the pause menu
	 */
    void toggleMenu();

    /**
	 * @brief Save the game, icluding metadata and chunks.
	 */
    void saveGame();

    /**
	 * @brief Load the game, including metadata and chunks.
	 */
    void loadGame();

    /**
     * @brief Respawn the player at the maps spawn position and angle.
     */
    void respawn();

    /**
	 * @brief Exit the game.
	 */
    void exit();

    /**
     * @brief Whether the player is currently in the menu.
     */
    bool menu = true;

private:
    /**
     * @brief Pointer to the map object in which the player is located.
     */
    Map* map;

    /**
     * @brief Position of the player in the map.
     */
    float pos[3];

    /**
	 * @brief Last chunk position of the player, used for updating chunks around the player when they move.
	 */
    int lastChunkPos[2];

    /**
     * @brief Angle of the player in the map.
     */
    float angle[2];

    /**
     * @brief Last cursor position.
     */
    float lastMouse[2];

    /**
     * @brief Sspeed at which the player is currently falling (<0 => the player is rising up).
     */
    float fallSpeed = 0;

    /**
     * @brief Delta time.
     */
    float deltaTime;

    /**
     * @brief Speed at which the player turns (mouse sensitivity).
     */
    float turnSpeed = 0.002f;

    /**
     * @brief Field of view of the camera in radians.
     */
    float fov = 60 * PI / 180.0f;

    /**
     * @brief timer for footstep sounds.
     */
    float stepTimer = 0.0f;

    /**
     * @brief Width of the screen in pixels.
     */
    int screenWidth = 1280;

    /**
     * @brief Height of the screen in pixels.
     */
    int screenHeight = 720;

    /**
     * @brief Render distance of the camera in voxels.
     */
    int renderDistance = 2;

    /**
     * @brief Flag for reseting the mouse position when exiting out of the menu.
     */
    bool resetMouse = false;

    /**
     * @brief Whether the window should use v-sync.
     */
    bool vSync = true;

    /**
     * @brief Whether the player is currently standing on the ground.
     */
    bool grounded = false;

    /**
     * @brief Buffer for the voxel in the middle of the screen, used fo world interaction.
     */
    GLuint hitBuffer;

    /**
     * @brief Sound system.
     */
    Sound* sound;

    /**
     * @brief Load shader source code from a file.
     *
     * @param filePath Path to the shader file.
     * 
     * @return The shader source code as a string.
     */
    std::string loadShaderSource(const std::string& filePath);

	/**
	 * @brief Respawn the player at the maps spawn position and angle.
	 */
    void resetPlayer();

    /**
     * @brief GLFW key callback.
     * 
     * @param window The GLFW window.
     * @param key The key that was pressed or released.
     * @param scancode The system-specific scancode of the key.
     * @param action The action (press, release, repeat).
     * @param mods Bit field describing which modifier keys were held down.
     */
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * @brief GLFW cursor position callback.
     * 
     * @param window The GLFW window.
     * @param xpos The new x-coordinate of the cursor.
     * @param ypos The new y-coordinate of the cursor.
     */
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);

    /**
     * @brief GLFW mouse button callback.
     * 
     * @param window The GLFW window.
     * @param button The mouse button that was pressed or released.
     * @param action The action (press, release).
     * @param mods Bit field describing which modifier keys were held down.
     */
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    /**
     * @brief GLFW window size callback.
     * 
     * @param window The GLFW window.
     * @param width The new width of the window in pixels.
     * @param height The new height of the window in pixels.
     */
    void windowSizeCallback(GLFWwindow* window, int width, int height);

    /**
     * @brief Check whether the player is colliding with the map at the specified location.
     *
     * @param pos The position to check for collision.
     * 
     * @return True if there is a collision, false otherwise.
     */
    bool checkPlayerCollision(float pos[3]);

    /**
     * @brief Move the player based on input and check for collisions.
     *
     * @param window The GLFW window.
     */
    void movePlayer(GLFWwindow* window);

    /**
     * @brief GLFW key callback, made static.
     * 
     * @param window The GLFW window.
     * @param key The key that was pressed or released.
     * @param scancode The system-specific scancode of the key.
     * @param action The action (press, release, repeat).
     * @param mods Bit field describing which modifier keys were held down.
     */
    static void staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

    /**
     * @brief GLFW cursor position callback, made static.
     * 
     * @param window The GLFW window.
     * @param xpos The new x-coordinate of the cursor.
     * @param ypos The new y-coordinate of the cursor.
     */
    static void staticMouseCallback(GLFWwindow* window, double xpos, double ypos);

    /**
     * @brief GLFW mouse button callback, made static.
     * 
     * @param window The GLFW window.
     * @param button The mouse button that was pressed or released.
     * @param action The action (press, release).
     * @param mods Bit field describing which modifier keys were held down.
     */
    static void staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

    /**
     * @brief GLFW window size callback, made static.
     * 
     * @param window The GLFW window.
     * @param width The new width of the window in pixels.
     * @param height The new height of the window in pixels.
     */
    static void staticWindowSizeCallback(GLFWwindow* window, int width, int height);

	/**
	 * @brief Pointer to the GLFW window.
	 */
    GLFWwindow* window = nullptr;

    /**
	 * @brief Pointer to the games voxel interactor, used for interacting with the world when placing and destroying blocks.
	 */
    IVoxelInteractor* voxelInteractor = nullptr;

	/**
	 * @brief Pointer to the engine GUI, used for rendering the menu GUI and passing it to the game GUI.
	 */
    EngineGui* engineGui = nullptr;
	
    /**
	 * @brief Pointer to the game GUI, used for rendering the menu GUI and passing it to the engine GUI.
	 */
    IGui* gameGui = nullptr;
};