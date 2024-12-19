#pragma once

#include "file_io.h"

// Ulozeni mapy do souboru
void saveFile(Map* map) {
    OPENFILENAMEW ofn;
    wchar_t path[260] = { }; // 260 - max. delka cesty souboru

    // nastaveni okna pro vyber souboru
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

    // okno pro vyber souboru
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

    // velikost mapy
    file.write(reinterpret_cast<char*>(&map->width), sizeof(map->width));
    file.write(reinterpret_cast<char*>(&map->height), sizeof(map->height));
    file.write(reinterpret_cast<char*>(&map->depth), sizeof(map->depth));

    // uhel kamery a hrace
    file.write(reinterpret_cast<char*>(&map->spawnAngle), 2 * sizeof(float));

    // rychlost pohybu ve scene
    file.write(reinterpret_cast<char*>(&map->moveSpeed), sizeof(map->moveSpeed));

    // pozice hrace
    file.write(reinterpret_cast<char*>(&map->spawnPos), 3 * sizeof(float));

    // smer slunce a barva nebe
    file.write(reinterpret_cast<char*>(&map->sunDir), 3 * sizeof(float));
    file.write(reinterpret_cast<char*>(&map->skyColor), 3 * sizeof(float));

    // mapa
    file.write(reinterpret_cast<char*>(map->voxelGridColor), (std::streamsize)map->width * map->height * map->depth * 4 * sizeof(float));
    file.write(reinterpret_cast<char*>(map->voxelGridProperties), (std::streamsize)map->width * map->height * map->depth * sizeof(float));

    file.close();
}

// Nacteni mapy ze souboru
void loadFile(Map* map, std::wstring filePath) {
    if (filePath.empty()) {
        OPENFILENAMEW ofn;
        wchar_t path[260] = { }; // 260 - max. delka cesty souboru

        // nastaveni okna pro vyber souboru
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

        // okno pro vyber souboru
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

    // velikost mapy
    file.read(reinterpret_cast<char*>(&map->width), sizeof(map->width));
    file.read(reinterpret_cast<char*>(&map->height), sizeof(map->height));
    file.read(reinterpret_cast<char*>(&map->depth), sizeof(map->depth));

    // uhel kamery a hrace
    file.read(reinterpret_cast<char*>(&map->spawnAngle), 2 * sizeof(float));

    // rychlost pohybu ve scene
    file.read(reinterpret_cast<char*>(&map->moveSpeed), sizeof(map->moveSpeed));

    // pozice hrace
    file.read(reinterpret_cast<char*>(&map->spawnPos), 3 * sizeof(float));

    // smer slunce a barva nebe
    file.read(reinterpret_cast<char*>(&map->sunDir), 3 * sizeof(float));
    file.read(reinterpret_cast<char*>(&map->skyColor), 3 * sizeof(float));

    // mapa
    map->voxelGridColor = new float[map->width * map->height * map->depth * 4];
    file.read(reinterpret_cast<char*>(map->voxelGridColor), (std::streamsize)map->width * map->height * map->depth * 4 * sizeof(float));

    map->voxelGridProperties = new float[map->width * map->height * map->depth];
    file.read(reinterpret_cast<char*>(map->voxelGridProperties), (std::streamsize)map->width * map->height * map->depth * sizeof(float));

    file.close();

    GLuint voxelGridColorTex, voxelGridPropertiesTex;

    // vytvoreni textur ktere obsahuji mapu
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