#pragma once

#include "file_io.h"

/**
 * @brief Save the map to the file selected in a file selection window.
 *
 * @param map Pointer to the map object to save.
 */
void saveFile(Map* map) {
    OPENFILENAMEW ofn;
    wchar_t path[260] = { }; // 260 - max filepath length

    // file selection wiundow setup
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = path;
    ofn.lpstrFile[0] = L'\0';
    ofn.nMaxFile = sizeof(path);
    ofn.lpstrFilter = L"All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

    // file selection window
    if (GetSaveFileNameW(&ofn) == FALSE) {
        std::cout << "No file selected\n";
        return;
    }

    std::wstring filePath = ofn.lpstrFile;

    std::ofstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Failed to open file for writing\n";
        return;
    }

    // map size
    file.write(reinterpret_cast<char*>(&map->width), sizeof(map->width));
    file.write(reinterpret_cast<char*>(&map->height), sizeof(map->height));
    file.write(reinterpret_cast<char*>(&map->depth), sizeof(map->depth));

    // player and camera angle
    file.write(reinterpret_cast<char*>(&map->spawnAngle), 2 * sizeof(float));

    // movement speed
    file.write(reinterpret_cast<char*>(&map->moveSpeed), sizeof(map->moveSpeed));

    // player position
    file.write(reinterpret_cast<char*>(&map->spawnPos), 3 * sizeof(float));

    // sun direction and sky color
    file.write(reinterpret_cast<char*>(&map->sunDir), 3 * sizeof(float));
    file.write(reinterpret_cast<char*>(&map->skyColor), 3 * sizeof(float));

    // map
    file.write(reinterpret_cast<char*>(map->voxelGridColor), (std::streamsize)map->width * map->height * map->depth * 4 * sizeof(float));
    file.write(reinterpret_cast<char*>(map->voxelGridProperties), (std::streamsize)map->width * map->height * map->depth * sizeof(float));
	file.write(reinterpret_cast<char*>(map->voxelGridCollision), (std::streamsize)map->width * map->height * map->depth * sizeof(bool));

    file.close();
}

/**
 * @brief Load the map from the optionally specified file or the file selected in a file selection window.
 *
 * @param map Pointer to the map object to load to.
 * @param filePath Optional path to the file to load from. If empty, a file selection window will be opened.
 */
void loadFile(Map* map, std::wstring filePath) {
    if (filePath.empty()) {
        OPENFILENAMEW ofn;
        wchar_t path[260] = { }; // 260 - max filepath length

        // file selection wiundow setup
        ZeroMemory(&ofn, sizeof(ofn));
        ofn.lStructSize = sizeof(ofn);
        ofn.hwndOwner = NULL;
        ofn.lpstrFile = path;
        ofn.lpstrFile[0] = L'\0';
        ofn.nMaxFile = sizeof(path);
        ofn.lpstrFilter = L"All Files\0*.*\0";
        ofn.nFilterIndex = 1;
        ofn.lpstrInitialDir = NULL;
        ofn.lpstrFileTitle = NULL;
        ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

        // file selection wiundow
        if (GetOpenFileNameW(&ofn) == FALSE) {
            std::cout << "No file selected\n";
            return;
        }

        filePath = ofn.lpstrFile;
    }

    std::ifstream file(filePath, std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Failed to open file for reading\n";
        return;
    }

    // map size
    file.read(reinterpret_cast<char*>(&map->width), sizeof(map->width));
    file.read(reinterpret_cast<char*>(&map->height), sizeof(map->height));
    file.read(reinterpret_cast<char*>(&map->depth), sizeof(map->depth));

    // player and camera angle
    file.read(reinterpret_cast<char*>(&map->spawnAngle), 2 * sizeof(float));

    // movement speed
    file.read(reinterpret_cast<char*>(&map->moveSpeed), sizeof(map->moveSpeed));

    // player position
    file.read(reinterpret_cast<char*>(&map->spawnPos), 3 * sizeof(float));

    // sun direction and sky color
    file.read(reinterpret_cast<char*>(&map->sunDir), 3 * sizeof(float));
    file.read(reinterpret_cast<char*>(&map->skyColor), 3 * sizeof(float));

    // map
    map->voxelGridColor = new float[map->width * map->height * map->depth * 4];
    file.read(reinterpret_cast<char*>(map->voxelGridColor), (std::streamsize)map->width * map->height * map->depth * 4 * sizeof(float));

    map->voxelGridProperties = new float[map->width * map->height * map->depth];
    file.read(reinterpret_cast<char*>(map->voxelGridProperties), (std::streamsize)map->width * map->height * map->depth * sizeof(float));

    map->voxelGridCollision = new bool[map->width * map->height * map->depth];
	file.read(reinterpret_cast<char*>(map->voxelGridCollision), (std::streamsize)map->width * map->height * map->depth * sizeof(bool));

    file.close();

	// map texture creation
    GLuint voxelGridColorTex, voxelGridPropertiesTex;

    glCreateTextures(GL_TEXTURE_3D, 1, &voxelGridColorTex);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, voxelGridColorTex);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, map->width, map->height, map->depth, 0, GL_RGBA, GL_FLOAT, map->voxelGridColor);
    glBindImageTexture(1, voxelGridColorTex, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);

    glCreateTextures(GL_TEXTURE_3D, 1, &voxelGridPropertiesTex);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridPropertiesTex, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, voxelGridPropertiesTex);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, map->width, map->height, map->depth, 0, GL_RED, GL_FLOAT, map->voxelGridProperties);
    glBindImageTexture(2, voxelGridPropertiesTex, 0, GL_TRUE, 0, GL_READ_ONLY, GL_R32F);
}