#include "world_generator.h"

#define STB_PERLIN_IMPLEMENTATION

#include "stb_perlin.h"

#define SEA_LEVEL 50

#define ROUGH_PERLIN_FREQ 0.004f
#define ROUGH_PERLIN_AMPL 40

#define FINE_PERLIN_FREQ 0.01f
#define FINE_PERLIN_AMPL 10

void WorldGenerator::generateChunk(Chunk& chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height)
{
    for (int z = 0; z < chunkDepth; z++) 
    {
        for (int x = 0; x < chunkWidth; x++) 
        {
            // perlin noise height
            float roughPerlin = stb_perlin_noise3((coord.second * chunkWidth + x) * ROUGH_PERLIN_FREQ, 0.0f, (coord.first * chunkDepth + z) * ROUGH_PERLIN_FREQ, 0, 0, 0) * FINE_PERLIN_AMPL;
            float finePerlin = stb_perlin_noise3((coord.second * chunkWidth + x) * FINE_PERLIN_FREQ, 0.0f, (coord.first * chunkDepth + z) * FINE_PERLIN_FREQ, 0, 0, 0) * FINE_PERLIN_AMPL;
            int terrainHeight = SEA_LEVEL + roughPerlin + finePerlin + 5;

            for (unsigned int y = 0; y < height; y++) 
            {
                size_t index = x * height * chunkDepth + (height - 1 - y) * chunkWidth + z;

                if (y <= terrainHeight) 
                {
					// below terrain
                    if (y >= terrainHeight - 4)
					{
						// high layers
                        if (terrainHeight > SEA_LEVEL + 4)
                        {
                            // high above sea level
                            if (y == terrainHeight)
                            {
                                // grass
                                chunk.voxelGridColor[index * 4 + 0] = 0.2f; // R
                                chunk.voxelGridColor[index * 4 + 1] = 0.8f; // G
                                chunk.voxelGridColor[index * 4 + 2] = 0.2f; // B
                            }
                            else
                            {
                                // dirt
                                chunk.voxelGridColor[index * 4 + 0] = 0.6f;
                                chunk.voxelGridColor[index * 4 + 1] = 0.4f;
                                chunk.voxelGridColor[index * 4 + 2] = 0.2f;
                            }
                        }
                        else if (terrainHeight > SEA_LEVEL - 4)
                        {
							// beaches
                            // sand
							chunk.voxelGridColor[index * 4 + 0] = 0.9f;
							chunk.voxelGridColor[index * 4 + 1] = 0.8f;
							chunk.voxelGridColor[index * 4 + 2] = 0.5f;
						}
                        else
                        {
                            // underwater
                            // gravel
							chunk.voxelGridColor[index * 4 + 0] = 0.4f;
							chunk.voxelGridColor[index * 4 + 1] = 0.4f;
							chunk.voxelGridColor[index * 4 + 2] = 0.4f;
                        }
					}
					else
                    {
                        // stone
						chunk.voxelGridColor[index * 4 + 0] = 0.5f;
						chunk.voxelGridColor[index * 4 + 1] = 0.5f;
						chunk.voxelGridColor[index * 4 + 2] = 0.5f;
                    }
                    chunk.voxelGridColor[index * 4 + 3] = 1.0f;
                    chunk.voxelGridProperties[index] = 0.0f;
                    chunk.voxelGridCollision[index] = true;
                }
                else 
                {
					// above terrain
                    if (y <= SEA_LEVEL) 
                    {
						// water
						chunk.voxelGridColor[index * 4 + 0] = 0.2f;
						chunk.voxelGridColor[index * 4 + 1] = 0.5f;
						chunk.voxelGridColor[index * 4 + 2] = 1.0f;
						chunk.voxelGridColor[index * 4 + 3] = 0.8f;
						chunk.voxelGridProperties[index] = 0.2f;
					}
                    else
                    {
                        // air
                        chunk.voxelGridColor[index * 4 + 0] = 0.0f;
                        chunk.voxelGridColor[index * 4 + 1] = 0.0f;
                        chunk.voxelGridColor[index * 4 + 2] = 0.0f;
                        chunk.voxelGridColor[index * 4 + 3] = 0.0f;
                        chunk.voxelGridProperties[index] = 0.0f;
                    }
                    chunk.voxelGridCollision[index] = false;
                }
            }
        }
    }
}
