#include "world_generator.h"

void WorldGenerator::generateChunk(Chunk& chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height) 
{
    for (unsigned int z = 0; z < chunkDepth; z++) {
        for (unsigned int x = 0; x < chunkWidth; x++) {
            unsigned int y = height - 1; // bottom layer
            size_t index = x * chunkWidth * height + y * chunkWidth + z;

            if ((coord.first + coord.second) % 2 == 0
            {
                chunk.voxelGridColor[index * 4 + 0] = 0.5f;
                chunk.voxelGridColor[index * 4 + 1] = 0.5f;
                chunk.voxelGridColor[index * 4 + 2] = 0.5f;
                chunk.voxelGridColor[index * 4 + 3] = 1.0f;
            }
            else
            {
                chunk.voxelGridColor[index * 4 + 0] = 0.6f;
                chunk.voxelGridColor[index * 4 + 1] = 0.6f;
                chunk.voxelGridColor[index * 4 + 2] = 0.6f;
                chunk.voxelGridColor[index * 4 + 3] = 1.0f;
            }

            chunk.voxelGridProperties[index] = 0.0f;

            chunk.voxelGridCollision[index] = true;
        }
    }
}
