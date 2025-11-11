#pragma once

#include <unordered_map>
#include <tuple>
#include <cstring>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "chunk.h"

struct ChunkCoordHash {
	std::size_t operator()(const std::pair<int, int>& k) const {
		return std::hash<int>()(k.first) ^ (std::hash<int>()(k.second) << 1);
	}
};

class Map {
public:
	/**
	 * @brief Width of the map in voxels.
	 */
	unsigned int chunkWidth;

	/**
	 * @brief Depth of the map in voxels.
	 */
	unsigned int chunkDepth;

	/**
	 * @brief Height of the map in voxels.
	 */
	unsigned int height;
	
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

	void updateChunks(int chunkPos[2], int renderDistance);
	Chunk& getChunk(int chunkX, int chunkZ);

private:
	std::unordered_map<std::pair<int, int>, Chunk, ChunkCoordHash> chunks;

	Chunk createChunk(std::pair<int, int> coord);
};