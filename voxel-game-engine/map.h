#pragma once

#include "chunk.h"
#include "chunk_generator.h"

#include <unordered_map>
#include <tuple>
#include <cstring>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

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

	std::wstring filePath;

	void updateChunks(int centerChunkPos[2], int renderDistance);
	void saveChunks();
	void changeVoxel(int pos[3], float voxel[5], bool collision);
	void setGenerator(IChunkGenerator* _generator);

	/**
	 * @brief Check whether the specified voxel is a collision voxel.
	 *
	 * @param pos The position of the voxel.
	 * @return True if there is a collision, false otherwise.
	 */
	bool checkCollision(int pos[3]);
	Chunk& getChunk(int chunkPos[2]);

private:
	std::unordered_map<std::pair<int, int>, Chunk, ChunkCoordHash> chunks;
	IChunkGenerator* generator = nullptr;

	Chunk createChunk(std::pair<int, int> coord);
};