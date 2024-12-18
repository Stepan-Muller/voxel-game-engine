#pragma once

struct Map {
	unsigned int width, 
				 height, 
				 depth,
				 moveSpeed;	
	float sunDir[3],
		  skyColor[3],
		  *voxelGridColor;
};