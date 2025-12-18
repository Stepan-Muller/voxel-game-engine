#include "world_generator.h"

#define STB_PERLIN_IMPLEMENTATION

#include "stb_perlin.h"

void WorldGenerator::generateChunk(Chunk& chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height)
{
    const float frequency = 0.004f;
    const unsigned int maxTerrainHeight = height - 1;

    for (int z = 0; z < chunkDepth; z++) {
        for (int x = 0; x < chunkWidth; x++) {
            // perlin noise height
            float perlin = (stb_perlin_noise3((coord.second * chunkWidth + x) * frequency, 0.0f, (coord.first * chunkDepth + z) * frequency, 0, 0, 0) + 1.0f) / 2.0f;
            int terrainHeight = perlin * 40;

            for (unsigned int y = 0; y < height; y++) {
                size_t index = x * height * chunkDepth + (height - 1 - y) * chunkWidth + z;

                if (y <= terrainHeight) {
                    // grass
                    if (y == terrainHeight) {
                        chunk.voxelGridColor[index * 4 + 0] = 0.2f; // R
                        chunk.voxelGridColor[index * 4 + 1] = 0.8f; // G
                        chunk.voxelGridColor[index * 4 + 2] = 0.2f; // B
                        chunk.voxelGridColor[index * 4 + 3] = 1.0f; // A
                    }
                    else {
                        // dirt
                        chunk.voxelGridColor[index * 4 + 0] = 0.6f;
                        chunk.voxelGridColor[index * 4 + 1] = 0.4f;
                        chunk.voxelGridColor[index * 4 + 2] = 0.2f;
                        chunk.voxelGridColor[index * 4 + 3] = 1.0f;
                    }

                    chunk.voxelGridProperties[index] = 0.0f;
                    chunk.voxelGridCollision[index] = true;
                }
                else {
                    // air
                    chunk.voxelGridColor[index * 4 + 0] = 0.0f;
                    chunk.voxelGridColor[index * 4 + 1] = 0.0f;
                    chunk.voxelGridColor[index * 4 + 2] = 0.0f;
                    chunk.voxelGridColor[index * 4 + 3] = 0.0f;
                    chunk.voxelGridProperties[index] = 0.0f;
                    chunk.voxelGridCollision[index] = false;
                }
            }
        }
    }
}
