#pragma once

#include "file_io.h"

void saveMeta(Map* map) {
    std::ofstream file((std::filesystem::path(map->filePath) / "meta.bin").string(), std::ios::binary);

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

    file.close();
}

void saveChunk(Map* map, int pos[2]) {
    std::ofstream file((std::filesystem::path(map->filePath) / "chunks" / ("chunk_" + std::to_string(pos[0]) + "_" + std::to_string(pos[1]) + ".bin")).string(), std::ios::binary);

    if (!file.is_open()) {
        std::cout << "Failed to open file for writing\n";
        return;
    }

    // map
    file.write(reinterpret_cast<char*>(map->getChunk(pos).voxelGridColor), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * 4 * sizeof(float));
    file.write(reinterpret_cast<char*>(map->getChunk(pos).voxelGridProperties), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * sizeof(float));
    file.write(reinterpret_cast<char*>(map->getChunk(pos).voxelGridCollision), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * sizeof(bool));

    file.close();
}

void loadMeta(Map* map, std::wstring filePath) {
    // file selection window
    if (filePath.empty()) {
        CoInitialize(NULL);
        std::wstring folder = selectFolder();
        CoUninitialize();

        if (folder.empty()) {
            std::cout << "No folder selected\n";
            return;
        }

        filePath = folder;
    }

    std::ifstream file((std::filesystem::path(filePath) / "meta.bin").string(), std::ios::binary);

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

    file.close();

    map->filePath = filePath;
}

bool loadChunk(Map* map, int pos[2], std::wstring filePath) {
    std::ifstream file((std::filesystem::path(map->filePath) / "chunks" / ("chunk_" + std::to_string(pos[0]) + "_" + std::to_string(pos[1]) + ".bin")).string(), std::ios::binary);

    // chunk was not saved yet
    if (!file.is_open()) {
        return false;
    }

    // map
    map->getChunk(pos).voxelGridColor = new float[map->chunkWidth * map->height * map->chunkDepth * 4];
    file.read(reinterpret_cast<char*>(map->getChunk(pos).voxelGridColor), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * 4 * sizeof(float));

    map->getChunk(pos).voxelGridProperties = new float[map->chunkWidth * map->height * map->chunkDepth];
    file.read(reinterpret_cast<char*>(map->getChunk(pos).voxelGridProperties), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * sizeof(float));

    map->getChunk(pos).voxelGridCollision = new bool[map->chunkWidth * map->height * map->chunkDepth];
    file.read(reinterpret_cast<char*>(map->getChunk(pos).voxelGridCollision), (std::streamsize)map->chunkWidth * map->height * map->chunkDepth * sizeof(bool));

    file.close();

    return true;
}

std::wstring selectFolder()
{
    std::wstring folderPath;

    // create the File Open Dialog
    IFileDialog* pFileDialog = nullptr;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, nullptr, CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&pFileDialog));

    if (SUCCEEDED(hr))
    {
        // pick folders only
        DWORD options;
        pFileDialog->GetOptions(&options);
        pFileDialog->SetOptions(options | FOS_PICKFOLDERS);

        // show dialog
        hr = pFileDialog->Show(NULL);
        if (SUCCEEDED(hr))
        {
            IShellItem* pItem = nullptr;
            hr = pFileDialog->GetResult(&pItem);
            if (SUCCEEDED(hr))
            {
                PWSTR pszFilePath = nullptr;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);
                if (SUCCEEDED(hr))
                {
                    folderPath = pszFilePath;
                    CoTaskMemFree(pszFilePath);
                }
                pItem->Release();
            }
        }
        pFileDialog->Release();
    }

    return folderPath;
}