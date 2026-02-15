#pragma once

#include "voxel_interactor.h"

#include "sound.h"

class GameVoxelInteractor : public IVoxelInteractor
{
public:
    /**
     * @brief Constructor for the GameVoxelInteractor class.
     * 
     * @param _map Pointer to the map object in which the player is located, used for interacting with the world when placing and destroying blocks.
     * @param _sound Pointer to the sound system, used for playing sounds when placing and
     */
    GameVoxelInteractor(Map *_map, Sound *_sound);

    /**
     * @brief Called when a voxel is placed in the world.
     * 
     * @param pos The position of the placed voxel.
     * @param lastChunkPos The position of the chunk that the player was in before placing.
     * @param renderDistance The current render distance, used for determining which chunks to update around the player when placing and destroying blocks.
     */
    void onVoxelPlace(int pos[3], int lastChunkPos[2], int renderDistance) override;

    /**
     * @brief Called when a voxel is destroyed in the world.
     * 
     * @param pos The position of the destroyed voxel.
     * @param lastChunkPos The position of the chunk that the player was in before destroying.
     * @param renderDistance The current render distance, used for determining which chunks to update around the player when placing and destroying blocks.
     */
    void onVoxelDestroy(int pos[3], int lastChunkPos[2], int renderDistance) override;

private:
    /**
     * @brief Pointer to the map object in which the player is located.
     */
    Map *map;

    /**
     * @brief Sound system.
     */
    Sound *sound;

    /**
     * @brief Buffer for the voxel in the middle of the screen, used fo world interaction.
     */
    float pickedVoxel[5] = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f};

    /**
     * @brief Whether the currently picked voxel is a collision voxel.
     */
    bool pickedVoxelCollision = false;

    /**
     * @brief Amount of voxels currently picked into the inventory.
     */
    int pickedAmount = 0;
};