#pragma once

#include "player.h"

Player::Player(Map* _map) {
	map = _map;

    std::string screenVertexSource = loadShaderSource("vertex.glsl");
    const GLchar* screenVertexShaderSource = screenVertexSource.c_str();
    
    std::string screenFragmentSource = loadShaderSource("fragment.glsl");
    const GLchar* screenFragmentShaderSource = screenFragmentSource.c_str();
    
    std::string screenComputeSource = loadShaderSource("compute.glsl");
    const GLchar* screenComputeShaderSource = screenComputeSource.c_str();

    GLuint screenTex;
    
    // Geometrie obrazovky
    GLfloat vertices[20] =
    {
        -1.0f, -1.0f , 0.0f, 0.0f, 0.0f,
        -1.0f,  1.0f , 0.0f, 0.0f, 1.0f,
         1.0f,  1.0f , 0.0f, 1.0f, 1.0f,
         1.0f, -1.0f , 0.0f, 1.0f, 0.0f,
    };
    GLuint indices[6] =
    {
        0, 2, 1,
        0, 3, 2
    };

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
    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vSync);

    // nastaveni inputù + zmìny velikosti okna
    glfwSetKeyCallback(window, staticKeyCallback);
    glfwSetCursorPosCallback(window, staticMouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowSizeCallback(window, staticWindowSizeCallback);

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

    // nabinduje geometrii do bufferù
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

    loadFile(map, L"demo.bin");

    /* Main game loop */
    while (!glfwWindowShouldClose(window))
    {
        // vypocet delta time
        deltaTime = (float)glfwGetTime() - lastTime;
        lastTime = (float)glfwGetTime();

        // pohyb hrace
        movePlayer(window);

        // inputy + zmìna velikosti obrazovky
        glfwPollEvents();

        // spusteni compute shaderu
        glUseProgram(computeProgram);
        // parametry compute shaderu
        glUniform1i(glGetUniformLocation(computeProgram, "renderDist"), renderDistance);
        glUniform2f(glGetUniformLocation(computeProgram, "angle"), angle[0], angle[1]);
        glUniform1f(glGetUniformLocation(computeProgram, "fov"), fov);
        glUniform3f(glGetUniformLocation(computeProgram, "playerPos"), pos[0], pos[1], pos[2]);
        glUniform3f(glGetUniformLocation(computeProgram, "sunDir"), map->sunDir[0], map->sunDir[1], map->sunDir[2]);
        glUniform3f(glGetUniformLocation(computeProgram, "skyColor"), map->skyColor[0], map->skyColor[1], map->skyColor[2]);

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

std::string Player::loadShaderSource(const std::string& filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void Player::respawn() {
	pos[0] = map->spawnPos[0];
	pos[1] = map->spawnPos[1];
	pos[2] = map->spawnPos[2];
	angle[0] = map->spawnAngle[0];
	angle[1] = map->spawnAngle[1];
	delta[0] = sin(angle[0]);
	delta[2] = cos(angle[0]);
    menu = false;
}

/* Kdyz je zmacknuta klavesa */
void Player::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
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
    if (menu && key == GLFW_KEY_S && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        saveFile(map);
        respawn();
    }

    // Nacteni mapy
    if (menu && key == GLFW_KEY_O && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        loadFile(map);
        respawn();
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        respawn();
}

/* Pohyb hrace pomoci mysi */
void Player::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    // pokud je v menu, nehybat s kamerou
    if (menu) return;

    if (resetMouse)
    {
        lastMouse[0] = (float)xpos;
        lastMouse[1] = (float)ypos;
        resetMouse = false;
    }

    // Osa x - otaceni hrace
    angle[0] = capRad360(angle[0] + ((float)xpos - lastMouse[0]) * turnSpeed);
    lastMouse[0] = (float)xpos;

    delta[0] = sin(angle[0]);
    delta[2] = cos(angle[0]);

    // Osa y - otaceni kamery
    angle[1] += capRad90_90(((float)ypos - lastMouse[1]) * turnSpeed);
    lastMouse[1] = (float)ypos;
}

/* Zmena velikosti okna */
void Player::windowSizeCallback(GLFWwindow* window, int width, int height)
{
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, screenWidth, screenHeight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
}

/* Pohyb hrace pomoci klaves WSAD */
void Player::movePlayer(GLFWwindow* window)
{
    // pokud je v menu, nehybat s hracem
    if (menu) return;

    int speedMultiplier = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ? 1 : 2;

    if (glfwGetKey(window, GLFW_KEY_W))
    {
        pos[0] -= delta[0] * deltaTime * map->moveSpeed * speedMultiplier;
        pos[2] -= delta[2] * deltaTime * map->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_S))
    {
        pos[0] += delta[0] * deltaTime * map->moveSpeed * speedMultiplier;
        pos[2] += delta[2] * deltaTime * map->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_A))
    {
        pos[0] += delta[2] * deltaTime * map->moveSpeed * speedMultiplier;
        pos[2] -= delta[0] * deltaTime * map->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_D))
    {
        pos[0] -= delta[2] * deltaTime * map->moveSpeed * speedMultiplier;
        pos[2] += delta[0] * deltaTime * map->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE))
        pos[1] -= deltaTime * map->moveSpeed * speedMultiplier;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT))
        pos[1] += deltaTime * map->moveSpeed * speedMultiplier;
}

void Player::staticKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Player* player = static_cast<Player*>(glfwGetWindowUserPointer(window));
    if (player) {
        player->keyCallback(window, key, scancode, action, mods);
    }
}

void Player::staticMouseCallback(GLFWwindow* window, double xpos, double ypos) {
    Player* player = static_cast<Player*>(glfwGetWindowUserPointer(window));
    if (player) {
        player->mouseCallback(window, xpos, ypos);
    }
}

void Player::staticWindowSizeCallback(GLFWwindow* window, int width, int height) {
    Player* player = static_cast<Player*>(glfwGetWindowUserPointer(window));
    if (player) {
        player->windowSizeCallback(window, width, height);
    }
}