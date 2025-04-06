#pragma once

struct Map {
	/**
	 * @brief Width of the map in voxels.
	 */
	unsigned int width;
	
	/**
	 * @brief Height of the map in voxels.
	 */
	unsigned int height;
	
	/**
	 * @brief Depth of the map in voxels.
	 */
	unsigned int depth;
	
	/**
	 * @brief The speed of the player in voxels per second.
	 */
	unsigned int moveSpeed;
	
	/**
	 * @brief Direction of the sun.
	 */
	float sunDir[3];
	
	/**
	 * @brief Colour of the sky.
	 */
	float skyColor[3];
	
	/**
	 * @brief Spawn position of the player.
	 */
	float spawnPos[3];
	
	/**
	 * @brief Spawn angle of the player.
	 */
	float spawnAngle[2];
	
	/**
	 * @brief Pointer to a grid of the first float voxel parameters (R, G, B, A).
	 */
	float *voxelGridColor;
	
	/**
	 * @brief Pointer to a grid of the second float voxel parameters (reflectivity).
	 */
	float *voxelGridProperties;
	
	/**
	 * @brief Pointer to a grid of the boolean voxel parameters (collision).
	 */
	bool *voxelGridCollision;
};