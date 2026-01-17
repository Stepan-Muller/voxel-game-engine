#pragma once

#include "game_voxel_interactor.h"

GameVoxelInteractor::GameVoxelInteractor(Map* _map)
{
	map = _map;
}

void GameVoxelInteractor::onVoxelPlace(int pos[3], int lastChunkPos[2], Gui* gui, int renderDistance)
{
    map->changeVoxel(pos, gui->selectedVoxel, gui->selectedVoxelCollision);
    map->updateChunks(lastChunkPos, renderDistance);
}

void GameVoxelInteractor::onVoxelDestroy(int pos[3], int lastChunkPos[2], int renderDistance)
{
	map->changeVoxel(pos, new float[5] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, false);
	map->updateChunks(lastChunkPos, renderDistance);
}