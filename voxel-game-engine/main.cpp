#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "util.c"

/* Konstanty hráče */
#define TURN_SPEED 0.002f
/* Proměnné hráče */
float playerPos[3], playerDelta[3], lastMouse[2], playerAngle, cameraAngle, deltaTime;
int moveSpeed;
bool menu = false, resetMouse = true;

/* Konstanty kamery */
#define V_SYNC true
/* Proměnné kamery */
float fov = 60 * PI / 180.0f;
int screenWidth = 1280, screenHeight = 720, renderDistance = 300;
float sunDir[3], skyColor[3];
GLuint voxelGridColorTex;

/* Proměnné mapy */
unsigned int gridWidth, gridHeight, gridDepth;
float * voxelGridColor;

/* Verze OpenGL (4.6) */
#define OPENGL_MAJOR_VERSION 4
#define OPENGL_MINOR_VERSION 6

/* Trojuhleníky přes celou obrazovku, na ně se renderuje raytracing */
GLuint screenTex;

GLfloat vertices[] =
{
    -1.0f, -1.0f , 0.0f, 0.0f, 0.0f,
    -1.0f,  1.0f , 0.0f, 0.0f, 1.0f,
     1.0f,  1.0f , 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f , 0.0f, 1.0f, 0.0f,
};

GLuint indices[] =
{
    0, 2, 1,
    0, 3, 2
};

/* OpenGL shadery */
std::string loadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string screenVertexSource = loadShaderSource("vertex.glsl");
const GLchar* screenVertexShaderSource = screenVertexSource.c_str();

std::string screenFragmentSource = loadShaderSource("fragment.glsl");
const GLchar* screenFragmentShaderSource = screenFragmentSource.c_str();

std::string screenComputeSource = loadShaderSource("compute.glsl");
const GLchar* screenComputeShaderSource = screenComputeSource.c_str();

// Ulozeni mapy do souboru
void saveFile() {
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
    file.write(reinterpret_cast<char*>(&gridWidth), sizeof(gridWidth));
    file.write(reinterpret_cast<char*>(&gridHeight), sizeof(gridHeight));
    file.write(reinterpret_cast<char*>(&gridDepth), sizeof(gridDepth));

    // uhel kamery a hrace
    file.write(reinterpret_cast<char*>(&playerAngle), sizeof(playerAngle));
    file.write(reinterpret_cast<char*>(&cameraAngle), sizeof(cameraAngle));

    // rychlost pohybu ve scene
    file.write(reinterpret_cast<char*>(&moveSpeed), sizeof(moveSpeed));

    // pozice hrace
    file.write(reinterpret_cast<char*>(&playerPos), 3 * sizeof(float));

    // smer slunce a barva nebe
    file.write(reinterpret_cast<char*>(&sunDir), 3 * sizeof(float));
    file.write(reinterpret_cast<char*>(&skyColor), 3 * sizeof(float));

    // mapa
    file.write(reinterpret_cast<char*>(voxelGridColor), (std::streamsize)gridWidth * gridHeight * gridDepth * 4 * sizeof(float));

    file.close();
}

// Nacteni mapy ze souboru
void loadFile(std::wstring filePath = L"") {
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
    file.read(reinterpret_cast<char*>(&gridWidth), sizeof(gridWidth));
    file.read(reinterpret_cast<char*>(&gridHeight), sizeof(gridHeight));
    file.read(reinterpret_cast<char*>(&gridDepth), sizeof(gridDepth));

    // uhel kamery a hrace
    file.read(reinterpret_cast<char*>(&playerAngle), sizeof(playerAngle));
    file.read(reinterpret_cast<char*>(&cameraAngle), sizeof(cameraAngle));

    // rychlost pohybu ve scene
    file.read(reinterpret_cast<char*>(&moveSpeed), sizeof(moveSpeed));

    // pozice hrace
    file.read(reinterpret_cast<char*>(&playerPos), 3 * sizeof(float));

    // smer slunce a barva nebe
    file.read(reinterpret_cast<char*>(&sunDir), 3 * sizeof(float));
    file.read(reinterpret_cast<char*>(&skyColor), 3 * sizeof(float));

    // mapa
    voxelGridColor = new float[gridWidth * gridHeight * gridDepth * 4];
    file.read(reinterpret_cast<char*>(voxelGridColor), (std::streamsize)gridWidth * gridHeight * gridDepth * 4 * sizeof(float));

    file.close();

    // vytvoreni textur ktere obsahuji mapu
    glCreateTextures(GL_TEXTURE_3D, 1, &voxelGridColorTex);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTextureParameteri(voxelGridColorTex, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_3D, voxelGridColorTex);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA32F, gridWidth, gridHeight, gridDepth, 0, GL_RGBA, GL_FLOAT, voxelGridColor);
    glBindImageTexture(1, voxelGridColorTex, 0, GL_TRUE, 0, GL_READ_ONLY, GL_RGBA32F);
}

/* Kdyz je zmacknuta klavesa */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Uvolneni mysi
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        menu = !menu;
        if (menu)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        resetMouse = true;
    }

    // Ulozeni mapy
    if (key == GLFW_KEY_S && action == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        saveFile();

    // Nacteni mapy
    if (key == GLFW_KEY_O && action == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        loadFile();
}

/* Pohyb hrace pomoci mysi */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // pokud je v menu, nehybat s kamerou
    if (menu)
        return;

    if (resetMouse)
    {
        lastMouse[0] = (float)xpos;
        lastMouse[1] = (float)ypos;
        resetMouse = false;
    }

    // Osa x - otaceni hrace
    playerAngle = capRad(playerAngle + ((float)xpos - lastMouse[0]) * TURN_SPEED);
    lastMouse[0] = (float)xpos;

    playerDelta[0] = sin(playerAngle);
    playerDelta[2] = cos(playerAngle);

    // Osa y - otaceni kamery
    cameraAngle += ((float)ypos - lastMouse[1]) * TURN_SPEED;
    lastMouse[1] = (float)ypos;

    if (cameraAngle > PI / 2)
        cameraAngle = PI / 2;
    else if (cameraAngle < -PI / 2)
        cameraAngle = -PI / 2;
}

/* Zmena velikosti okna */
void window_size_callback(GLFWwindow* window, int width, int height)
{
    screenWidth = width;
    screenHeight = height;
    
    glViewport(0, 0, screenWidth, screenHeight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
}

/* Pohyb hrace pomoci klaves WSAD */
void movePlayer(GLFWwindow* window)
{
    // pokud je v menu, nehybat s hracem
    if (menu)
        return;

    int speedMultiplier = 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
		speedMultiplier = 2;

    if (glfwGetKey(window, GLFW_KEY_W))
    {
        playerPos[0] -= playerDelta[0] * deltaTime * moveSpeed * speedMultiplier;
        playerPos[2] -= playerDelta[2] * deltaTime * moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_S))
    {
        playerPos[0] += playerDelta[0] * deltaTime * moveSpeed * speedMultiplier;
        playerPos[2] += playerDelta[2] * deltaTime * moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_A))
    {
        playerPos[0] += playerDelta[2] * deltaTime * moveSpeed * speedMultiplier;
        playerPos[2] -= playerDelta[0] * deltaTime * moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_D))
    {
        playerPos[0] -= playerDelta[2] * deltaTime * moveSpeed * speedMultiplier;
        playerPos[2] += playerDelta[0] * deltaTime * moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE))
        playerPos[1] -= deltaTime * moveSpeed * speedMultiplier;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        playerPos[1] += deltaTime * moveSpeed * speedMultiplier;
}

//int main(int argc, char* argv[]) {                                                                                        // pro konzolovou aplikaci
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) { // pro aplikaci s oknem
    glfwInit();

    // nastavení verze OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // tvorba okna
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Voxel Game Engine", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(V_SYNC);

    // nastaveni inputů + změny velikosti okna
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowSizeCallback(window, window_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
    }
    glViewport(0, 0, screenWidth, screenHeight);

    /*
    VAO - Vertex Array Object
    VBO - Vertex Buffer Object
    EBO - Element Buffer Object
    */
    GLuint VAO, VBO, EBO;
    glCreateVertexArrays(1, &VAO);
    glCreateBuffers(1, &VBO);
    glCreateBuffers(1, &EBO);

    // nabinduje geometrii do bufferů
    glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glEnableVertexArrayAttrib(VAO, 1);
    glVertexArrayAttribBinding(VAO, 1, 0);
    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

    // nabinduje VBO a EBO na VAO
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
    glVertexArrayElementBuffer(VAO, EBO);

    // nastevení textury
    glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
    glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

    // screen shadery
    GLuint screenVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(screenVertexShader, 1, &screenVertexShaderSource, NULL);
    glCompileShader(screenVertexShader);
    GLuint screenFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(screenFragmentShader, 1, &screenFragmentShaderSource, NULL);
    glCompileShader(screenFragmentShader);

    GLuint screenShaderProgram = glCreateProgram();
    glAttachShader(screenShaderProgram, screenVertexShader);
    glAttachShader(screenShaderProgram, screenFragmentShader);
    glLinkProgram(screenShaderProgram);

    glDeleteShader(screenVertexShader);
    glDeleteShader(screenFragmentShader);

    // compute shader
    GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(computeShader, 1, &screenComputeShaderSource, NULL);
    glCompileShader(computeShader);

    GLuint computeProgram = glCreateProgram();
    glAttachShader(computeProgram, computeShader);
    glLinkProgram(computeProgram);

    glDeleteShader(computeShader);

    // priprava na delta time
    float lastTime = (float)glfwGetTime();

    loadFile(L"demo.bin");

    /* Main game loop */
    while (!glfwWindowShouldClose(window))
    {
        // vypocet delta time
        deltaTime = (float)glfwGetTime() - lastTime;
        lastTime = (float)glfwGetTime();

        // pohyb hrace
        movePlayer(window);

        // inputy + změna velikosti obrazovky
        glfwPollEvents();

        // spusteni compute shaderu
        glUseProgram(computeProgram);
        // parametry compute shaderu
        glUniform1i(glGetUniformLocation(computeProgram, "renderDist"), renderDistance);
        glUniform1f(glGetUniformLocation(computeProgram, "playerAngle"), playerAngle);
        glUniform1f(glGetUniformLocation(computeProgram, "cameraAngle"), cameraAngle);
        glUniform1f(glGetUniformLocation(computeProgram, "fov"), fov);
        glUniform3f(glGetUniformLocation(computeProgram, "playerPos"), playerPos[0], playerPos[1], playerPos[2]);
        glUniform3f(glGetUniformLocation(computeProgram, "sunDir"), sunDir[0], sunDir[1], sunDir[2]);
        glUniform3f(glGetUniformLocation(computeProgram, "skyColor"), skyColor[0], skyColor[1], skyColor[2]);

        // velikost
        glDispatchCompute(screenWidth / 8 + 1, screenHeight / 4 + 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // screen shader
        glUseProgram(screenShaderProgram);
        // prametry screen shaderu
        glBindTextureUnit(0, screenTex);
        glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
}