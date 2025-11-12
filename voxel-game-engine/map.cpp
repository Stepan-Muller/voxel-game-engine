#pragma once

#include "map.h"

#include "file_io.h"

void Map::updateChunks(int centerChunkPos[2], int renderDistance)
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
    for (int x = 0; x <= 2 * renderDistance; x++)
        for (int z = 0; z <= 2 * renderDistance; z++)
        {
            int chunkPos[2] = { centerChunkPos[0] + x - renderDistance, centerChunkPos[1] + z - renderDistance };
            glTexSubImage3D(GL_TEXTURE_3D, 0, x * chunkWidth, 0, z * chunkDepth, chunkWidth, height, chunkDepth, GL_RGBA, GL_FLOAT, getChunk(chunkPos).voxelGridColor);
        }
    glBindImageTexture(1, voxelGridColorTex, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    glCreateTextures(GL_TEXTURE_3D, 1, &voxelGridPropertiesTex);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, voxelGridPropertiesTex);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, chunkWidth * (2 * renderDistance + 1), height, chunkDepth * (2 * renderDistance + 1), 0, GL_RED, GL_FLOAT, nullptr);
    for (int x = 0; x <= 2 * renderDistance; x++)
        for (int z = 0; z <= 2 * renderDistance; z++)
        {
			int chunkPos[2] = { centerChunkPos[0] + x - renderDistance, centerChunkPos[1] + z - renderDistance };
            glTexSubImage3D(GL_TEXTURE_3D, 0, x * chunkWidth, 0, z * chunkDepth, chunkWidth, height, chunkDepth, GL_RED, GL_FLOAT, getChunk(chunkPos).voxelGridProperties);
        }
    glBindImageTexture(2, voxelGridPropertiesTex, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);

    // remove distant chunks
    for (auto it = chunks.begin(); it != chunks.end(); ) {
        int x = it->first.first;
        int z = it->first.second;
        if (abs(x - centerChunkPos[0]) > renderDistance + 1 || abs(z - centerChunkPos[1]) > renderDistance + 1) {
            // save chunks if edited
            if (it->second.edited) {
                int chunkPos[2] = { it->first.first, it->first.second };
                saveChunk(this, chunkPos);
            }
            
            delete[] it->second.voxelGridColor;
            delete[] it->second.voxelGridProperties;
            delete[] it->second.voxelGridCollision;
            it = chunks.erase(it);
        }
        else {
            ++it;
        }
    }
}

void Map::saveChunks()
{
    for (auto it = chunks.begin(); it != chunks.end(); it++) {
        int x = it->first.first;
        int z = it->first.second;

        // save chunks if edited
        if (it->second.edited) {
            int chunkPos[2] = { it->first.first, it->first.second };
            saveChunk(this, chunkPos);
        }
    }
}

void Map::changeVoxel(int pos[3], float voxel[5], bool collision)
{
    int chunkPos[2] = { floor((float)pos[0] / chunkWidth), floor((float)pos[2] / chunkWidth) };

	pos[0] = pos[0] - chunkPos[0] * chunkWidth;
	pos[2] = pos[2] - chunkPos[1] * chunkWidth;
    
    getChunk(chunkPos).voxelGridColor[pos[0] * 4 + pos[1] * chunkWidth * 4 + pos[2] * chunkWidth * height * 4] = voxel[0];
    getChunk(chunkPos).voxelGridColor[pos[0] * 4 + pos[1] * chunkWidth * 4 + pos[2] * chunkWidth * height * 4 + 1] = voxel[1];
    getChunk(chunkPos).voxelGridColor[pos[0] * 4 + pos[1] * chunkWidth * 4 + pos[2] * chunkWidth * height * 4 + 2] = voxel[2];
    getChunk(chunkPos).voxelGridColor[pos[0] * 4 + pos[1] * chunkWidth * 4 + pos[2] * chunkWidth * height * 4 + 3] = voxel[3];
    getChunk(chunkPos).voxelGridProperties[pos[0] + pos[1] * chunkWidth + pos[2] * chunkWidth * height] = voxel[4];
    getChunk(chunkPos).voxelGridCollision[pos[0] + pos[1] * chunkWidth + pos[2] * chunkWidth * height] = collision;

	getChunk(chunkPos).edited = true;
}

bool Map::checkCollision(int pos[3])
{
    // prevent the player from falling under the map
    if (pos[1] >= (int)height)
        return true;

    if (pos[1] < 0)
        return false;

    int chunkPos[2] = { floor((float)pos[0] / chunkWidth), floor((float)pos[2] / chunkWidth) };
	
    pos[0] = pos[0] - chunkPos[0] * chunkWidth;
    pos[2] = pos[2] - chunkPos[1] * chunkWidth;

    return getChunk(chunkPos).voxelGridCollision[pos[0] + pos[1] * chunkWidth + pos[2] * chunkWidth * height];
}

// Get a chunk at (x,z), create it if it doesn't exist
Chunk& Map::getChunk(int chunkPos[2]) {
    std::pair<int, int> coord = { chunkPos[0], chunkPos[1] };

    auto it = chunks.find(coord);
    if (it != chunks.end()) {
        return it->second;
    }

	chunks[coord] = Chunk();

    // load an existing chunk
	if (loadChunk(this, chunkPos, filePath)) return chunks[coord];

	// create a new chunk
    Chunk newChunk = createChunk(coord);
    chunks[coord] = newChunk;
    return chunks[coord];
}

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
