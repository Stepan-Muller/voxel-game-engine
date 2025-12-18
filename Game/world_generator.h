#pragma once

#include "chunk_generator.h"

class WorldGenerator : public IChunkGenerator {
public:
    void generateChunk(Chunk& chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height);
};
