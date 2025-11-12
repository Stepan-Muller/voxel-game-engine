#pragma once

#include "file_io.h"

void saveMap(Map* map) {
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
    file.write(reinterpret_cast<char*>(&map->chunkWidth), sizeof(map->chunkWidth));
    file.write(reinterpret_cast<char*>(&map->height), sizeof(map->height));
    file.write(reinterpret_cast<char*>(&map->chunkDepth), sizeof(map->chunkDepth));

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
    //file.write(reinterpret_cast<char*>(map->getChunk(0, 0).voxelGridColor), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * 4 * sizeof(float));
    //file.write(reinterpret_cast<char*>(map->getChunk(0, 0).voxelGridProperties), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * sizeof(float));
	//file.write(reinterpret_cast<char*>(map->getChunk(0, 0).voxelGridCollision), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * sizeof(bool));

    file.close();
}

void loadMap(Map* map, std::wstring filePath) {
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
    file.read(reinterpret_cast<char*>(&map->chunkWidth), sizeof(map->chunkWidth));
    file.read(reinterpret_cast<char*>(&map->height), sizeof(map->height));
    file.read(reinterpret_cast<char*>(&map->chunkDepth), sizeof(map->chunkDepth));

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
    //map->getChunk(0, 0).voxelGridColor = new float[map->chunkWidth * map->height * map->chunkDepth * 4];
    //file.read(reinterpret_cast<char*>(map->getChunk(0, 0).voxelGridColor), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * 4 * sizeof(float));

    //map->getChunk(0, 0).voxelGridProperties = new float[map->chunkWidth * map->height * map->chunkDepth];
    //file.read(reinterpret_cast<char*>(map->getChunk(0, 0).voxelGridProperties), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * sizeof(float));

    //map->getChunk(0, 0).voxelGridCollision = new bool[map->chunkWidth * map->height * map->chunkDepth];
	//file.read(reinterpret_cast<char*>(map->getChunk(0, 0).voxelGridCollision), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * sizeof(bool));

    file.close();
}