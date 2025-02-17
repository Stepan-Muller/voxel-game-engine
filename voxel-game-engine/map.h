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
		  *voxelGridColor, // �erven�, zelen�, modr�, pr�hlednost
		  *voxelGridProperties; // odrazivost
	bool *voxelGridCollision; // kolize
};