#include <iostream>
#include <fstream>
#include <sstream>
#include <windows.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "util.c"
#include "file_io.h"
#include "player.h"
#include "map.h"

Map map;
Player player;

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

/* Kdyz je zmacknuta klavesa */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // Uvolneni mysi
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        player.menu = !player.menu;
        if (player.menu)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        player.resetMouse = true;
    }

    // Ulozeni mapy
    if (key == GLFW_KEY_S && action == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        saveFile(&map, &player, &player.camera);

    // Nacteni mapy
    if (key == GLFW_KEY_O && action == GLFW_PRESS &&
        glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        loadFile(&map, &player, &player.camera);
}

/* Pohyb hrace pomoci mysi */
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    // pokud je v menu, nehybat s kamerou
    if (player.menu)
        return;

    if (player.resetMouse)
    {
        player.lastMouse[0] = (float)xpos;
        player.lastMouse[1] = (float)ypos;
        player.resetMouse = false;
    }

    // Osa x - otaceni hrace
    player.angle[0] = capRad(player.angle[0] + ((float)xpos - player.lastMouse[0]) * player.turnSpeed);
    player.lastMouse[0] = (float)xpos;

    player.delta[0] = sin(player.angle[0]);
    player.delta[2] = cos(player.angle[0]);

    // Osa y - otaceni kamery
    player.angle[1] += ((float)ypos - player.lastMouse[1]) * player.turnSpeed;
    player.lastMouse[1] = (float)ypos;

    if (player.angle[1] > PI / 2)
        player.angle[1] = PI / 2;
    else if (player.angle[1] < -PI / 2)
        player.angle[1] = -PI / 2;
}

/* Zmena velikosti okna */
void window_size_callback(GLFWwindow* window, int width, int height)
{
    player.camera.width = width;
    player.camera.height = height;
    
    glViewport(0, 0, player.camera.width, player.camera.height);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
}

/* Pohyb hrace pomoci klaves WSAD */
void movePlayer(GLFWwindow* window)
{
    // pokud je v menu, nehybat s hracem
    if (player.menu)
        return;

    int speedMultiplier = 1;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL))
		speedMultiplier = 2;

    if (glfwGetKey(window, GLFW_KEY_W))
    {
        player.pos[0] -= player.delta[0] * player.deltaTime * player.moveSpeed * speedMultiplier;
        player.pos[2] -= player.delta[2] * player.deltaTime * player.moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_S))
    {
        player.pos[0] += player.delta[0] * player.deltaTime * player.moveSpeed * speedMultiplier;
        player.pos[2] += player.delta[2] * player.deltaTime * player.moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_A))
    {
        player.pos[0] += player.delta[2] * player.deltaTime * player.moveSpeed * speedMultiplier;
        player.pos[2] -= player.delta[0] * player.deltaTime * player.moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_D))
    {
        player.pos[0] -= player.delta[2] * player.deltaTime * player.moveSpeed * speedMultiplier;
        player.pos[2] += player.delta[0] * player.deltaTime * player.moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE))
        player.pos[1] -= player.deltaTime * player.moveSpeed * speedMultiplier;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        player.pos[1] += player.deltaTime * player.moveSpeed * speedMultiplier;
}

//int main(int argc, char* argv[]) {                                                                                        // pro konzolovou aplikaci
int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) { // pro aplikaci s oknem
    glfwInit();

    // nastavení verze OpenGL
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // tvorba okna
    GLFWwindow* window = glfwCreateWindow(player.camera.width, player.camera.height, "Voxel Game Engine", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(player.camera.vSync);

    // nastaveni inputů + změny velikosti okna
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowSizeCallback(window, window_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize OpenGL context" << std::endl;
    }
    glViewport(0, 0, player.camera.width, player.camera.height);

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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, player.camera.width, player.camera.height, 0, GL_RGBA, GL_FLOAT, nullptr);
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

    loadFile(&map, &player, &player.camera, L"demo.bin");

    /* Main game loop */
    while (!glfwWindowShouldClose(window))
    {
        // vypocet delta time
        player.deltaTime = (float)glfwGetTime() - lastTime;
        lastTime = (float)glfwGetTime();

        // pohyb hrace
        movePlayer(window);

        // inputy + změna velikosti obrazovky
        glfwPollEvents();

        // spusteni compute shaderu
        glUseProgram(computeProgram);
        // parametry compute shaderu
        glUniform1i(glGetUniformLocation(computeProgram, "renderDist"), player.camera.renderDistance);
        glUniform2f(glGetUniformLocation(computeProgram, "angle"), player.angle[0], player.angle[1]);
        glUniform1f(glGetUniformLocation(computeProgram, "fov"), player.camera.fov);
        glUniform3f(glGetUniformLocation(computeProgram, "playerPos"), player.pos[0], player.pos[1], player.pos[2]);
        glUniform3f(glGetUniformLocation(computeProgram, "sunDir"), player.camera.sunDir[0], player.camera.sunDir[1], player.camera.sunDir[2]);
        glUniform3f(glGetUniformLocation(computeProgram, "skyColor"), player.camera.skyColor[0], player.camera.skyColor[1], player.camera.skyColor[2]);

        // velikost
        glDispatchCompute(player.camera.width / 8 + 1, player.camera.height / 4 + 1, 1);
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