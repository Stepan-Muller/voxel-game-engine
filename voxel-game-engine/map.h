#pragma once

#include "chunk.h"
#include "chunk_generator.h"

#include <unordered_map>
#include <tuple>
#include <cstring>
#include <string>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

struct ChunkCoordHash
{
	std::size_t operator()(const std::pair<int, int> &k) const
	{
		return std::hash<int>()(k.first) ^ (std::hash<int>()(k.second) << 1);
	}
};

class Map
{
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

	/**
	 * @brief File path for saving and loading chunks.
	 */
	std::wstring filePath;

	/**
	 * @brief Update the chunks around the player based on the current render distance and center chunk position.
	 * 
	 * @param centerChunkPos The chunk coordinates of the center chunk around which to update the chunks.
	 * @param renderDistance The render distance in chunks, determining how many chunks around the center chunk should be loaded and generated.
	 */
	void updateChunks(int centerChunkPos[2], int renderDistance);

	/**
	 * @brief Save all edited chunks to disk.
	 */
	void saveChunks();

	/**
	 * @brief Change the voxel at the specified position to the specified parameters and mark the chunk as edited.
	 *
	 * @param pos The position of the voxel to change (voxel coordinates, not chunk coordinates).
	 * @param voxel An array of 5 floats representing the new parameters of the voxel (R, G, B, A, reflectivity).
	 * @param collision Whether the voxel should be a collision voxel or not.
	 */
	void changeVoxel(int pos[3], float voxel[5], bool collision);

	/**
	 * @brief Set the chunk generator to use for generating new chunks when they are loaded or when the player moves into an area that has not been generated yet.
	 * 
	 * @param _generator Pointer to the chunk generator to use.
	 */
	void setGenerator(IChunkGenerator *_generator);

	/**
	 * @brief Check whether the specified voxel is a collision voxel.
	 *
	 * @param pos The position of the voxel.
	 * 
	 * @return True if there is a collision, false otherwise.
	 */
	bool checkCollision(int pos[3]);

	/**
	 * @brief Get a reference to the chunk at the specified chunk coordinates. If the chunk does not exist, it will be created and generated using the chunk generator.
	 * 
	 * @param chunkPos The chunk coordinates of the chunk to get (chunk grid coordinates, not voxel coordinates).
	 * 
	 * @return Reference to the chunk at the specified coordinates.
	 */
	Chunk &getChunk(int chunkPos[2]);

private:
	/**
	 * @brief Map of chunk coordinates to chunks. The key is a pair of integers representing the chunk's x and z coordinates, and the value is the corresponding Chunk object.
	 */
	std::unordered_map<std::pair<int, int>, Chunk, ChunkCoordHash> chunks;

	/**
	 * @brief Pointer to the chunk generator used for generating new chunks when they are loaded or when the player moves into an area that has not been generated yet.
	 */
	IChunkGenerator *generator = nullptr;

	/**
	 * @brief Create a new chunk at the specified coordinates by generating it using the chunk generator and adding it to the map. If a chunk already exists at the specified coordinates, it will be returned instead.
	 * 
	 * @param coord The chunk coordinates of the chunk to create (chunk grid coordinates, not voxel coordinates).
	 * 
	 * @return The created or existing chunk at the specified coordinates.
	 */
	Chunk createChunk(std::pair<int, int> coord);
};