#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "util.c"
#include "map.h"

/* Verze OpenGL (4.6) */
#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 6

class Player {
public:
    Player(Map* _map);
private:
    Map* map;
    float pos[3], 
          delta[3], 
          angle[2],
          lastMouse[2], 
          deltaTime, 
          turnSpeed = 0.002f, 
          fov = 60 * PI / 180.0f;
    int screenWidth = 1280,
        screenHeight = 720,
        renderDistance = 300;
    bool menu = false,
         resetMouse = true,
         vSync = true;
    GLuint voxelGridColorTex;
    std::string loadShaderSource(const std::string& filePath);
    void saveFile();
    void loadFile(std::wstring filePath = L"");
    void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void mouseCallback(GLFWwindow* window, double xpos, double ypos);
    void windowSizeCallback(GLFWwindow* window, int width, int height);
    void movePlayer(GLFWwindow* window);
    static void staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void staticMouseCallback(GLFWwindow* window, double xpos, double ypos);
    static void staticWindowSizeCallback(GLFWwindow* window, int width, int height);
};