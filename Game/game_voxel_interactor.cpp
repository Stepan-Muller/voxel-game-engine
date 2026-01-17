#pragma once

#include "game_voxel_interactor.h"

GameVoxelInteractor::GameVoxelInteractor(Map* _map)
{
	map = _map;
}

void GameVoxelInteractor::onVoxelPlace(int pos[3], int lastChunkPos[2], int renderDistance)
{
    map->changeVoxel(pos, pickedVoxel, pickedVoxelCollision);
    map->updateChunks(lastChunkPos, renderDistance);

	pickedVoxel[0] = 0.0f;
	pickedVoxel[1] = 0.0f;
	pickedVoxel[2] = 0.0f;
	pickedVoxel[3] = 0.0f;
	pickedVoxel[4] = 0.0f;
	pickedVoxelCollision = false;
}

void GameVoxelInteractor::onVoxelDestroy(int pos[3], int lastChunkPos[2], int renderDistance)
{
	if (pickedVoxel[0] != 0.0f || pickedVoxel[1] != 0.0f || pickedVoxel[2] != 0.0f || pickedVoxel[3] != 0.0f || pickedVoxel[4] != 0.0f || pickedVoxelCollision)
		return;
	
	int chunkPos[2] = {floor((float)pos[0] / map->chunkWidth), floor((float)pos[2] / map->chunkWidth)};

	pickedVoxel[0] = map->getChunk(chunkPos).voxelGridColor[(pos[0] - chunkPos[0] * map->chunkWidth) * 4 + pos[1] * map->chunkWidth * 4 + (pos[2] - chunkPos[1] * map->chunkWidth) * map->chunkWidth * map->height * 4];
	pickedVoxel[1] = map->getChunk(chunkPos).voxelGridColor[(pos[0] - chunkPos[0] * map->chunkWidth) * 4 + pos[1] * map->chunkWidth * 4 + (pos[2] - chunkPos[1] * map->chunkWidth) * map->chunkWidth * map->height * 4 + 1];
	pickedVoxel[2] = map->getChunk(chunkPos).voxelGridColor[(pos[0] - chunkPos[0] * map->chunkWidth) * 4 + pos[1] * map->chunkWidth * 4 + (pos[2] - chunkPos[1] * map->chunkWidth) * map->chunkWidth * map->height * 4 + 2];
	pickedVoxel[3] = map->getChunk(chunkPos).voxelGridColor[(pos[0] - chunkPos[0] * map->chunkWidth) * 4 + pos[1] * map->chunkWidth * 4 + (pos[2] - chunkPos[1] * map->chunkWidth) * map->chunkWidth * map->height * 4 + 3];
	pickedVoxel[4] = map->getChunk(chunkPos).voxelGridProperties[(pos[0] - chunkPos[0] * map->chunkWidth) + pos[1] * map->chunkWidth + (pos[2] - chunkPos[1] * map->chunkWidth) * map->chunkWidth * map->height];
	pickedVoxelCollision = map->getChunk(chunkPos).voxelGridCollision[(pos[0] - chunkPos[0] * map->chunkWidth) + pos[1] * map->chunkWidth + (pos[2] - chunkPos[1] * map->chunkWidth) * map->chunkWidth * map->height];

	map->changeVoxel(pos, new float[5] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, false);
	map->updateChunks(lastChunkPos, renderDistance);
}