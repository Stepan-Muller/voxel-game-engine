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

/**
 * @brief Save the metadata of the map.
 * 
 * @param map Pointer to the map object containing the metadata to save.
 */
void saveMeta(Map *map);

/**
 * @brief Save a chunk of the map.
 * 
 * @param map Pointer to the map object containing the chunk to save.
 * @param pos The position of the chunk to save, specified as an array of two integers representing the chunk's x and z coordinates.
 */
void saveChunk(Map *map, int pos[2]);

/**
 * @brief Load the metadata of the map.
 * 
 * @param map Pointer to the map object where the loaded metadata will be stored.
 * @param filePath Optional file path to load the metadata from. If not provided, a file selection dialog will be shown to the user to select a folder containing the metadata.
 */
void loadMeta(Map *map, std::wstring filePath = L"");

/**
 * @brief Load a chunk of the map.
 * 
 * @param map Pointer to the map object where the loaded chunk will be stored.
 * @param pos The position of the chunk to load, specified as an array of two integers representing the chunk's x and z coordinates.
 * @param filePath Optional file path to load the chunk from. If not provided, the chunk will be loaded from the default file path specified in the map object.
 * 
 * @return True if the chunk was successfully loaded, false otherwise (e.g., if the chunk file does not exist).
 */
bool loadChunk(Map *map, int pos[2], std::wstring filePath = L"");

/**
 * @brief Select a folder using the Windows file dialog.
 * 
 * @return The path of the selected folder as a wide string. If no folder was selected, an empty string is returned.
 */
std::wstring selectFolder();