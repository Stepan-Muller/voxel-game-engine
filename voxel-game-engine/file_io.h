#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "map.h"

void saveFile(Map* map),
	 loadFile(Map* map, std::wstring filePath = L"");