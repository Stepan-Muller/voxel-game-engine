#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <windows.h>
#include <shobjidl.h>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "map.h"

void saveMeta(Map* map);
void saveChunk(Map* map, int pos[2]);
void loadMeta(Map* map, std::wstring filePath = L"");
bool loadChunk(Map* map, int pos[2], std::wstring filePath = L"");
std::wstring selectFolder();