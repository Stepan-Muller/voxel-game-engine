#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "map.h"
#include "camera.h"
#include "player.h"

void saveFile(Map* map, Player* player, Camera* camera);
void loadFile(Map* map, Player* player, Camera* camera, std::wstring filePath = L"");
