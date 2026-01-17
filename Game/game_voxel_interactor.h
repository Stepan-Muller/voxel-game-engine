#pragma once

#include "gui.h"

#include "voxel_interactor.h"

class GameVoxelInteractor : public IVoxelInteractor {
public:
    GameVoxelInteractor(Map* _map);

    void onVoxelPlace(int pos[3], int lastChunkPos[2], Gui* gui, int renderDistance) override;

    void onVoxelDestroy(int pos[3], int lastChunkPos[2], int renderDistance) override;

private:
    Map* map;
};