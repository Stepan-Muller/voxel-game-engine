#pragma once

#include "glad/glad.h"

#include "util.c"

struct Camera {
	float fov = 60 * PI / 180.0f;
	int width = 1280, height = 720, renderDistance = 300;
	float sunDir[3], skyColor[3];
	bool vSync = true;
	GLuint voxelGridColorTex;
};