#pragma once

struct Chunk {
	/**
	 * @brief Pointer to a grid of the first float voxel parameters (R, G, B, A).
	 */
	float* voxelGridColor;

	/**
	 * @brief Pointer to a grid of the second float voxel parameters (reflectivity).
	 */
	float* voxelGridProperties;

	/**
	 * @brief Pointer to a grid of the boolean voxel parameters (collision).
	 */
	bool* voxelGridCollision;
};