#pragma once

#include "chunk_generator.h"
#include "map.h"

class WorldGenerator : public IChunkGenerator
{
public:
    /**
     * @brief Constructor for the WorldGenerator class.
     * 
     * @param _map Pointer to the map object for which this generator will generate chunks.
     */
    WorldGenerator(Map *_map);

    /**
     * @brief Generate a chunk at the specified coordinates.
     * 
     * @param chunk Reference to the chunk object to be generated.
     * @param coord The x and z coordinates of the chunk to be generated.
     * @param chunkWidth The width of the chunk in voxels.
     * @param chunkDepth The depth of the chunk in voxels.
     * @param height The height of the chunk in voxels.
     */
    void generateChunk(Chunk &chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height);

private:
    /**
     * @brief Pointer to the map object for which this generator is generating chunks.
     */
    Map map;

    /**
     * @brief Generate a tree at the specified location within the chunk.
     * 
     * @param chunk Reference to the chunk object in which the tree will be generated.
     * @param coord The x and z coordinates of the chunk in which the tree will be generated.
     * @param baseX The x coordinate of the base of the tree within the chunk.
     * @param baseY The y coordinate of the base of the tree within the chunk.
     * @param baseZ The z coordinate of the base of the tree within the chunk.
     * @param chunkWidth The width of the chunk in voxels.
     * @param chunkDepth The depth of the chunk in voxels.
     * @param height The height of the chunk in voxels.
     */
    void generateTree(Chunk &chunk, std::pair<int, int> coord, int baseX, int baseY, int baseZ, int chunkWidth, int chunkDepth, int height);

    /**
     * @brief Place a voxel in the chunk with the specified parameters.
     * 
     * @param chunk Reference to the chunk object in which the voxel will be placed.
     * @param coord The x and z coordinates of the chunk in which the voxel will be placed.
     * @param x The x coordinate of the voxel within the chunk.
     * @param y The y coordinate of the voxel within the chunk.
     * @param z The z coordinate of the voxel within the chunk.
     * @param chunkWidth The width of the chunk in voxels.
     * @param chunkDepth The depth of the chunk in voxels.
     * @param height The height of the chunk in voxels.
     * @param color The RGBA color of the voxel, as an array of 4 floats from 0.0 to 1.0.
     * @param property The reflectivity property of the voxel, as a float from 0.0 to 1.0.
     * @param collision Whether the voxel is a collision voxel, as a boolean.
     */
    void placeVoxel(Chunk &chunk, std::pair<int, int> coord, int x, int y, int z, int chunkWidth, int chunkDepth, int height, const float color[4], float property, bool collision);
};
