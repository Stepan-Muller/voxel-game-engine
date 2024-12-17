#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "camera.h"
#include "map.h"

struct Player {
public:
    float pos[3], delta[3], lastMouse[2], angle[2], deltaTime, turnSpeed = 0.002f;
    int moveSpeed;
    bool menu = false, resetMouse = true;
    Camera camera;
};
