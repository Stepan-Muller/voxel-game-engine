#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "util.c"
#include "map.h"
#include "file_io.h"
#include "sound.h"
#include "gui.h"

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
     */
    Player(Map* _map);
private:
    /**
	 * @brief Pointer to the map object in which the player is located.
     */
    Map map;
    
    /**
	 * @brief Position of the player in the map.
     */
    float pos[3];

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
     * @brief Whether the player is currently in the menu.
     */
    bool menu = false;
    
    /**
     * @brief Flag for reseting the mouse position when exiting out of the menu.
     */
    bool resetMouse = true;
    
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
    Sound sound;
    
    /**
     * @brief Pointer to the gui class.
     */
    Gui* gui;

    /**
     * @brief Load shader source code from a file.
     *
     * @param filePath Path to the shader file.
     * @return The shader source code as a string.
     */
    std::string loadShaderSource(const std::string& filePath);
    
    /**
     * @brief Respawn the player at the maps spawn position and angle.
     */
    void respawn();
    
    /**
     * @brief GLFW key callback.
     */
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    /**
     * @brief GLFW cursor position callback.
     */
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    
    /**
     * @brief GLFW mouse button callback.
     */
    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    
    /**
     * @brief GLFW window size callback.
     */
    void windowSizeCallback(GLFWwindow* window, int width, int height);
    
    /**
     * @brief Check whether the player is colliding with the map at the specified location.
     *
     * @param pos The position to check for collision.
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
     */
    static void staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    
    /**
     * @brief GLFW cursor position callback, made static.
     */
    static void staticMouseCallback(GLFWwindow* window, double xpos, double ypos);
    
    /**
     * @brief GLFW mouse button callback, made static.
     */
    static void staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    
    /**
     * @brief GLFW window size callback, made static.
     */
    static void staticWindowSizeCallback(GLFWwindow* window, int width, int height);
};