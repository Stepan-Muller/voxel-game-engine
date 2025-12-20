#include "world_generator.h"

#define STB_PERLIN_IMPLEMENTATION

#include <random>
#include <cmath>
#include "stb_perlin.h"
#include <iostream>

#define SEA_LEVEL 50

#define TEMPERATURE_PERLIN_FREQ 0.0005f

#define ROUGHNNESS_PERLIN_FREQ 0.0005f

#define SMOOTH_PERLIN_FREQ 0.004f
#define SMOOTH_PERLIN_AMPL 40.0f

#define ROUGH_PERLIN_FREQ 0.01f
#define ROUGH_PERLIN_AMPL 100.0f

#define FINE_PERLIN_FREQ 0.01f
#define FINE_PERLIN_AMPL 10.0f

// simple hash for std::pair<int,int>
struct PairHash {
    std::size_t operator()(const std::pair<int, int>& p) const noexcept {
        std::size_t h1 = std::hash<int>{}(p.first);
        std::size_t h2 = std::hash<int>{}(p.second);
        return h1 ^ (h2 << 1);
    }
};

WorldGenerator::WorldGenerator(Map* _map)
{
    map = *_map;
}

#define MAX_TREES_PER_CHUNK 1

void WorldGenerator::generateChunk(Chunk& chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height)
{
    unsigned int seed = PairHash{}(coord);
    std::mt19937 rng(seed);
    int maxX = std::max(0, chunkWidth - 1);
    std::uniform_int_distribution<int> xDist(0, maxX);
    int maxZ = std::max(0, chunkDepth - 1);
    std::uniform_int_distribution<int> zDist(0, maxZ);

    std::vector<std::pair<int, int>> trees;

    for (int i = 0; i < MAX_TREES_PER_CHUNK; i++)
    {
        int treeX = xDist(rng);
        int treeZ = zDist(rng);
        trees.emplace_back(treeX, treeZ);
    }

    for (int z = 0; z < chunkDepth; z++) 
    {
        for (int x = 0; x < chunkWidth; x++) 
        {
			float temperature = stb_perlin_noise3((coord.second * chunkWidth + x) * TEMPERATURE_PERLIN_FREQ, 696969.0f, (coord.first * chunkDepth + z) * TEMPERATURE_PERLIN_FREQ, 0, 0, 0);

			float roughness = (stb_perlin_noise3((coord.second * chunkWidth + x) * ROUGHNNESS_PERLIN_FREQ, 676767.0f, (coord.first * chunkDepth + z) * ROUGHNNESS_PERLIN_FREQ, 0, 0, 0) + 1.0f) / 2.0f;
            
            // perlin noise for terrain height
			float smoothPerlin = stb_perlin_noise3((coord.second * chunkWidth + x) * SMOOTH_PERLIN_FREQ, 42069.0f, (coord.first * chunkDepth + z) * SMOOTH_PERLIN_FREQ, 0, 0, 0) * SMOOTH_PERLIN_AMPL;
            float roughPerlin = stb_perlin_noise3((coord.second * chunkWidth + x) * ROUGH_PERLIN_FREQ, 420420.0f, (coord.first * chunkDepth + z) * ROUGH_PERLIN_FREQ, 0, 0, 0) * FINE_PERLIN_AMPL;
            float finePerlin = stb_perlin_noise3((coord.second * chunkWidth + x) * FINE_PERLIN_FREQ, 0.0f, (coord.first * chunkDepth + z) * FINE_PERLIN_FREQ, 0, 0, 0) * FINE_PERLIN_AMPL;
            int terrainHeight = SEA_LEVEL + smoothPerlin + roughPerlin * roughness + finePerlin + 5;

            // check if current position is a tree base
            bool isTree = false;
            for (auto& treePos : trees)
            {
                if (treePos.first == x && treePos.second == z)
                {
                    isTree = true;
                    break;
                }
            }

            for (unsigned int y = 0; y < height; y++) 
            {
                size_t index = x * height * chunkDepth + (height - 1 - y) * chunkWidth + z;

                if (y <= terrainHeight) 
                {
					// below terrain
                    if (y >= terrainHeight - 4)
					{
						// high layers
                        if (terrainHeight > SEA_LEVEL + 4 && temperature > 0.0f)
                        {
                            // high above sea level
                            if (y == terrainHeight)
                            {
                                // grass
                                chunk.voxelGridColor[index * 4 + 0] = 0.2f; // R
                                chunk.voxelGridColor[index * 4 + 1] = 0.8f; // G
                                chunk.voxelGridColor[index * 4 + 2] = 0.2f; // B
                                
                                // tree generation
                                if (isTree)
									generateTree(chunk, coord, x, y - 1, z, chunkWidth, chunkDepth, height);

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
							// beaches/desserts
                            // sand
							chunk.voxelGridColor[index * 4 + 0] = 0.9f;
							chunk.voxelGridColor[index * 4 + 1] = 0.8f;
							chunk.voxelGridColor[index * 4 + 2] = 0.5f;
						}
                        else
                        {
                            // deep underwater
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
                        chunk.voxelGridCollision[index] = false;
					}
                }
            }
        }
    }
}

void WorldGenerator::placeVoxel(Chunk& chunk, std::pair<int, int> coord, int x, int y, int z, int chunkWidth, int chunkDepth, int height, const float color[4], float property, bool collision)
{
    if (x >= 0 && x < chunkWidth && z >= 0 && z < chunkDepth && y >= 0 && y < height)
    {
        size_t index = x * height * chunkDepth + (height - 1 - y) * chunkWidth + z;

        chunk.voxelGridColor[index * 4 + 0] = color[0];
        chunk.voxelGridColor[index * 4 + 1] = color[1];
        chunk.voxelGridColor[index * 4 + 2] = color[2];
        chunk.voxelGridColor[index * 4 + 3] = color[3];

        chunk.voxelGridProperties[index] = property;
        chunk.voxelGridCollision[index] = collision;
    }
    else
    {
        int globalPos[3] = {coord.second * chunkWidth + x, y, coord.first * chunkDepth + z};

        float voxelData[5] = {color[0], color[1], color[2], color[3], property};

        map.changeVoxel(globalPos, voxelData, collision);
    }
}


#define TREE_TRUNK_HEIGHT 25
#define TREE_TRUNK_RADIUS 1
#define TREE_LEAF_RADIUS 7

void WorldGenerator::generateTree(Chunk& chunk, std::pair<int, int> coord, int baseX, int baseY, int baseZ, int chunkWidth, int chunkDepth, int height)
{
    // trunk
    for (int y = baseY; y < baseY + TREE_TRUNK_HEIGHT && y < height; y++)
    {
        for (int dx = -TREE_TRUNK_RADIUS; dx <= TREE_TRUNK_RADIUS; dx++)
        {
            for (int dz = -TREE_TRUNK_RADIUS; dz <= TREE_TRUNK_RADIUS; dz++)
            {
                int lx = baseX + dx;
                int lz = baseZ + dz;

                float trunkColor[4] = { 0.55f, 0.27f, 0.07f, 1.0f };
                placeVoxel(chunk, coord, lx, y, lz, chunkWidth, chunkDepth, height, trunkColor, 0.0f, true);
            }
        }
    }

    // leaves
    int leafBaseY = baseY + TREE_TRUNK_HEIGHT - TREE_LEAF_RADIUS * 2;
    int leafTopY = baseY + TREE_TRUNK_HEIGHT;
    for (int y = leafBaseY; y <= leafTopY && y < height; y++)
    {
        for (int dx = -TREE_LEAF_RADIUS; dx <= TREE_LEAF_RADIUS; dx++)
        {
            for (int dz = -TREE_LEAF_RADIUS; dz <= TREE_LEAF_RADIUS; dz++)
            {
                int lx = baseX + dx;
                int lz = baseZ + dz;

                float leafColor[4] = { 0.0f, 0.5f, 0.0f, 1.0f };
                placeVoxel(chunk, coord, lx, y, lz, chunkWidth, chunkDepth, height, leafColor, 0.0f, true);
            }
        }
    }
}