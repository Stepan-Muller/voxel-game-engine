#pragma once

struct Map {
	unsigned int width,
				 height,
				 depth,
				 moveSpeed;
	float sunDir[3],
		  skyColor[3],
		  spawnPos[3],
		  spawnAngle[2],
		  *voxelGridColor, // èervená, zelená, modrá, prùhlednost
		  *voxelGridProperties; // odrazivost
	bool *voxelGridCollision; // kolize
};