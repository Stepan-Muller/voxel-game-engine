#include "player.h"
#include "map.h"

//int main(int argc, char* argv[]) {                                                                                        // pro konzolovou aplikaci
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) { // pro aplikaci s oknem
    Map map;
    Player player(&map);
}