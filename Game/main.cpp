#pragma once

#include "player.h"
#include "map.h"
#include "world_generator.h"
#include "game_voxel_interactor.h"
#include "game_gui.h"
#include "sound.h"

//int main(int argc, char* argv[]) {                                                                                        // pro konzolovou aplikaci
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) { // pro aplikaci s oknem
    Map map;
    WorldGenerator generator(&map);

	map.setGenerator(&generator);

	Sound sound;
	GameVoxelInteractor voxelInteractor(&map, &sound);
	GameGui gameGui(&sound);

    Player player(&map, &voxelInteractor, &gameGui, &sound);
}