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
    Player(Map* _map);
private:
    Map* map;
	float pos[3], 
          angle[2],
          lastMouse[2],
          fallSpeed = 0,
          deltaTime, 
          turnSpeed = 0.002f, 
          fov = 60 * PI / 180.0f,
          stepTimer = 0.0f;
    int screenWidth = 1280,
        screenHeight = 720,
        renderDistance = 300;
	bool menu = false,
         resetMouse = true,
         vSync = true,
	     grounded = false;
    GLuint hitBuffer;
    Sound sound;
    Gui* gui;

    std::string loadShaderSource(const std::string& filePath);
    void respawn(),
         keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods),
         mouseCallback(GLFWwindow* window, double xpos, double ypos),
         mouseButtonCallback(GLFWwindow* window, int button, int action, int mods),
         windowSizeCallback(GLFWwindow* window, int width, int height),
         movePlayer(GLFWwindow* window),
         changeVoxel(int pos[3], float voxel[5], bool collision);
	bool checkCollision(int pos[3]),
         checkPlayerCollision(float pos[3]);
    static void staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods),
                staticMouseCallback(GLFWwindow* window, double xpos, double ypos),
                staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods),
                staticWindowSizeCallback(GLFWwindow* window, int width, int height);
};