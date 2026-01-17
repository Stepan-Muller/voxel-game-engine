#pragma once

#include "voxel_interactor.h"

class GameVoxelInteractor : public IVoxelInteractor {
public:
    GameVoxelInteractor(Map* _map);

    void onVoxelPlace(int pos[3], int lastChunkPos[2], int renderDistance) override;

    void onVoxelDestroy(int pos[3], int lastChunkPos[2], int renderDistance) override;

private:
    Map* map;

	float pickedVoxel[5] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	bool pickedVoxelCollision = false;
};