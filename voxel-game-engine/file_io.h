#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "map.h"

/**
 * @brief Save the map to the file selected in a file selection window.
 *
 * @param map Pointer to the map object to save.
 */
void saveMap(Map* map);

/**
 * @brief Load the map from the optionally specified file or the file selected in a file selection window.
 *
 * @param map Pointer to the map object to load to.
 * @param filePath Optional path to the file to load from. If empty, a file selection window will be opened.
 */
void loadMap(Map* map, std::wstring filePath = L"");