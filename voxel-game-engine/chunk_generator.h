#pragma once

#include "chunk.h"
#include <utility>

class IChunkGenerator
{
public:
    /**
     * @brief Destructor for the IChunkGenerator interface.
     */
    virtual ~IChunkGenerator() = default;

    /**
     * @brief Generate a chunk at the specified coordinates.
     * 
     * @param chunk Reference to the chunk object to fill with generated data.
     * @param coord Coordinates of the chunk to generate (chunk grid coordinates, not voxel coordinates).
     * @param chunkWidth Width of the chunk in voxels.
     * @param chunkDepth Depth of the chunk in voxels.
     * @param height Height of the chunk in voxels.
     */
    virtual void generateChunk(Chunk &chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height) = 0;
};
