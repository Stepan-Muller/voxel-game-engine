#pragma once

#include "map.h"

class IVoxelInteractor {
public:
    virtual ~IVoxelInteractor() = default;

    virtual void onVoxelPlace(int pos[3], int lastChunkPos[2], int renderDistance) = 0;

    virtual void onVoxelDestroy(int pos[3], int lastChunkPos[2], int renderDistance) = 0;
};
