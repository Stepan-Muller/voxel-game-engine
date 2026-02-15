#pragma once

#include "map.h"

class IVoxelInteractor
{
public:
    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~IVoxelInteractor() = default;

    /**
     * @brief Called when a voxel is placed in the world.
     * 
     * @param pos The position of the placed voxel (x, y, z).
     * @param lastChunkPos The position of the last chunk that was interacted with (x, z).
     * @param renderDistance The current render distance of the world.
     */
    virtual void onVoxelPlace(int pos[3], int lastChunkPos[2], int renderDistance) = 0;

    /**
     * @brief Called when a voxel is destroyed in the world.
     * 
     * @param pos The position of the destroyed voxel (x, y, z).
     * @param lastChunkPos The position of the last chunk that was interacted with (x, z).
     * @param renderDistance The current render distance of the world.
     */
    virtual void onVoxelDestroy(int pos[3], int lastChunkPos[2], int renderDistance) = 0;
};
