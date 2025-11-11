#pragma once

#include "map.h"

Chunk Map::createChunk(std::pair<int, int> coord) {
    Chunk chunk;

    chunk.voxelGridColor = new float[chunkWidth * height * chunkDepth * 4];
    chunk.voxelGridProperties = new float[chunkWidth * height * chunkDepth];
    chunk.voxelGridCollision = new bool[chunkWidth * height * chunkDepth];

    for (size_t i = 0; i < chunkWidth * height * chunkDepth * 4; i++) chunk.voxelGridColor[i] = 0.0f;
    for (size_t i = 0; i < chunkWidth * height * chunkDepth; i++) chunk.voxelGridProperties[i] = 0.0f;
    for (size_t i = 0; i < chunkWidth * height * chunkDepth; i++) chunk.voxelGridCollision[i] = false;

    for (unsigned int z = 0; z < chunkDepth; z++) {
        for (unsigned int x = 0; x < chunkWidth; x++) {
            unsigned int y = height - 1; // bottom layer
            size_t index = x * chunkWidth * height + y * chunkWidth + z;

            // RGBA color (solid gray floor)
            if ((coord.first + coord.second) % 2 == 0)
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

            // Properties: reflectivity (example 0.0)
            chunk.voxelGridProperties[index] = 0.0f;

            // Collision
            chunk.voxelGridCollision[index] = true;
        }
    }

    return chunk;
}

void Map::updateChunks(int chunkPos[2], int renderDistance)
{
    // map texture creation
    GLuint voxelGridColorTex, voxelGridPropertiesTex;

    glCreateTextures(GL_TEXTURE_3D, 1, &voxelGridColorTex);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, voxelGridColorTex);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, chunkWidth * (2 * renderDistance + 1), height, chunkDepth * (2 * renderDistance + 1), 0, GL_RGBA, GL_FLOAT, nullptr);
    for (int x = 0; x <= (2 * renderDistance + 1); x++)
        for (int z = 0; z <= (2 * renderDistance + 1); z++)
            glTexSubImage3D(GL_TEXTURE_3D, 0, x * chunkWidth, 0, z * chunkDepth, chunkWidth, height, chunkDepth, GL_RGBA, GL_FLOAT, getChunk(chunkPos[0] + x - renderDistance, chunkPos[1] + z - renderDistance).voxelGridColor);
    glBindImageTexture(1, voxelGridColorTex, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    glCreateTextures(GL_TEXTURE_3D, 1, &voxelGridPropertiesTex);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, voxelGridPropertiesTex);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, chunkWidth * (2 * renderDistance + 1), height, chunkDepth * (2 * renderDistance + 1), 0, GL_RED, GL_FLOAT, nullptr);
    for (int x = 0; x <= (2 * renderDistance + 1); x++)
        for (int z = 0; z <= (2 * renderDistance + 1); z++)
            glTexSubImage3D(GL_TEXTURE_3D, 0, x * chunkWidth, 0, z * chunkDepth, chunkWidth, height, chunkDepth, GL_RED, GL_FLOAT, getChunk(chunkPos[0] + x - renderDistance, chunkPos[1] + z - renderDistance).voxelGridProperties);
    glBindImageTexture(2, voxelGridPropertiesTex, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
}

// Get a chunk at (x,z), create it if it doesn't exist
Chunk& Map::getChunk(int x, int z) {
    std::pair<int, int> coord = { x, z };

    auto it = chunks.find(coord);
    if (it != chunks.end()) {
        return it->second;
    }

    // vytvoøit neexistující chunk
    Chunk newChunk = createChunk(coord);
    chunks[coord] = newChunk;
    return chunks[coord];
}
