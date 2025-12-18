#pragma once

#include "chunk.h"
#include <utility>

class IChunkGenerator {
public:
    virtual ~IChunkGenerator() = default;

    virtual void generateChunk(Chunk& chunk, std::pair<int, int> coord, int chunkWidth, int chunkDepth, int height) = 0;
};
