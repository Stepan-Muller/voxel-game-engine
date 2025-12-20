#pragma once

#include "chunk_generator.h"
#include "map.h"

class WorldGenerator : public IChunkGenerator {
public:
    WorldGenerator(Map* _map);
    
    void generateChunk(Chunk& chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height);

private:
    Map map;

    void generateTree(Chunk& chunk, std::pair<int, int> coord, int baseX, int baseY, int baseZ, int chunkWidth, int chunkDepth, int height);
    void placeVoxel(Chunk& chunk, std::pair<int, int> coord, int x, int y, int z, int chunkWidth, int chunkDepth, int height, const float color[4], float property, bool collision);
};
