#pragma once

#include "player.h"

Player::Player(Map* _map) {
	map = _map;

    std::string screenVertexSource = loadShaderSource("shaders/vertex.glsl");
    const GLchar* screenVertexShaderSource = screenVertexSource.c_str();
    
    std::string screenFragmentSource = loadShaderSource("shaders/fragment.glsl");
    const GLchar* screenFragmentShaderSource = screenFragmentSource.c_str();
    
    std::string screenComputeSource = loadShaderSource("shaders/compute.glsl");
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
	glfwSetMouseButtonCallback(window, staticMouseButtonCallback);
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

	// buffer pro voxel ve stredu obrazovky
    glGenBuffers(1, &hitBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 6 * sizeof(int), NULL, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, hitBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

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

    // inicializace gui
	gui = new Gui(window);

    // priprava na delta time
    float lastTime = (float)glfwGetTime();

    loadFile(map, L"demo.bin");
    respawn();

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

        glDispatchCompute(screenWidth / 8 + 1, screenHeight / 4 + 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // screen shader
        glUseProgram(screenShaderProgram);
        // prametry screen shaderu
        glBindTextureUnit(0, screenTex);
        glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

        // UI
		if (menu) gui->render();

        glfwSwapBuffers(window);
    }

    // cleanup glfw
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
}

// R, G, B, A, odrazivost; kolize
void Player::changeVoxel(int pos[3], float voxel[5], bool collision)
{
	if (pos[0] < 0 || pos[1] < 0 || pos[2] < 0 || pos[0] >= (int)map->width || pos[1] >= (int)map->height || pos[2] >= (int)map->depth)
		return;
    
    map->voxelGridColor[pos[0] * 4 + pos[1] * map->width * 4 + pos[2] * map->width * map->height * 4] = voxel[0];
    map->voxelGridColor[pos[0] * 4 + pos[1] * map->width * 4 + pos[2] * map->width * map->height * 4 + 1] = voxel[1];
    map->voxelGridColor[pos[0] * 4 + pos[1] * map->width * 4 + pos[2] * map->width * map->height * 4 + 2] = voxel[2];
    map->voxelGridColor[pos[0] * 4 + pos[1] * map->width * 4 + pos[2] * map->width * map->height * 4 + 3] = voxel[3];
	map->voxelGridProperties[pos[0] + pos[1] * map->width + pos[2] * map->width * map->height] = voxel[4];
	map->voxelGridCollision[pos[0] + pos[1] * map->width + pos[2] * map->width * map->height] = collision;

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
        saveFile(map);

    // Nacteni mapy
    if (menu && key == GLFW_KEY_O && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        loadFile(map);
        respawn();
    }

    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        respawn();

    // test systemu zvuku
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
    {
        soundSystem.playSound("sounds/test.wav");
    }
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

    // Osa y - otaceni kamery
    angle[1] += capRad90_90(((float)ypos - lastMouse[1]) * turnSpeed);
    lastMouse[1] = (float)ypos;
}

void Player::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    if (menu) return;
    
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
	{
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitBuffer);
        int hitVoxel[3] = { -1, -1, -1 };
		int hitNormal[3] = { 0, 0, 0 };

        int* ptr = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        if (ptr) {
            hitVoxel[0] = ptr[0];
            hitVoxel[1] = ptr[1];
            hitVoxel[2] = ptr[2];
			hitNormal[0] = ptr[3];
			hitNormal[1] = ptr[4];
			hitNormal[2] = ptr[5];
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		int hitNeighbour[3] = { hitVoxel[0] - hitNormal[0], hitVoxel[1] - hitNormal[1], hitVoxel[2] - hitNormal[2] };

		changeVoxel(hitNeighbour, gui->selectedVoxel, gui->selectedVoxelCollision);
	}
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitBuffer);
        int hitVoxel[3] = { -1, -1, -1 };

        int* ptr = (int*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
        if (ptr) {
            hitVoxel[0] = ptr[0];
            hitVoxel[1] = ptr[1];
            hitVoxel[2] = ptr[2];
            glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
        }
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        changeVoxel(hitVoxel, new float[5] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, false);
    }
}

/* Zmena velikosti okna */
void Player::windowSizeCallback(GLFWwindow* window, int width, int height)
{
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, screenWidth, screenHeight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
}

bool Player::checkCollision(int pos[3]) 
{
    // zamezit hraci spadnout pod mapu
    if (pos[1] >= (int)map->height)
        return true;
    
    if (pos[0] < 0 || pos[1] < 0 || pos[2] < 0 || pos[0] >= (int)map->width || pos[2] >= (int)map->depth)
		return false;
    
    return map->voxelGridCollision[pos[0] + pos[1] * map->width + pos[2] * map->width * map->height];
}

// zjisti, zda je na dane pozici kolize
bool Player::checkPlayerCollision(float pos[3]) 
{
    int min[3];
	int max[3];
    
    min[0] = (int)(pos[0] - 2.5f);
    max[0] = (int)(pos[0] + 2.5f);
    min[1] = (int)pos[1];
    max[1] = (int)(pos[1] + 10.0f);
    min[2] = (int)(pos[2] - 2.5f);
    max[2] = (int)(pos[2] + 2.5f);

    for (int x = min[0]; x <= max[0]; x++)
    {
        for (int y = min[1]; y <= max[1]; y++)
        {
            for (int z = min[2]; z <= max[2]; z++)
            {
                int voxelPos[3] = {x, y, z};
                
                if (checkCollision(voxelPos))
                    return true;
            }
        }
    }

    return false;
}

/* Pohyb hrace pomoci klaves WSAD */
void Player::movePlayer(GLFWwindow* window)
{
    // pokud je v menu, nehybat s hracem
    if (menu) return;

    int speedMultiplier = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ? 1 : 2;

    float move[3] = {0, 0, 0};

    float delta[2];
    delta[0] = sin(angle[0]);
    delta[1] = cos(angle[0]);

    if (glfwGetKey(window, GLFW_KEY_W))
    {
        move[0] -= delta[0] * deltaTime * map->moveSpeed * speedMultiplier;
        move[2] -= delta[1] * deltaTime * map->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_S))
    {
        move[0] += delta[0] * deltaTime * map->moveSpeed * speedMultiplier;
        move[2] += delta[1] * deltaTime * map->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_A))
    {
        move[0] += delta[1] * deltaTime * map->moveSpeed * speedMultiplier;
        move[2] -= delta[0] * deltaTime * map->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_D))
    {
        move[0] -= delta[1] * deltaTime * map->moveSpeed * speedMultiplier;
        move[2] += delta[0] * deltaTime * map->moveSpeed * speedMultiplier;
    }

    if (grounded)
    {
		fallSpeed = 0;
        if (glfwGetKey(window, GLFW_KEY_SPACE))
			fallSpeed = -50;
	}
	else
	{
		fallSpeed += GRAVITY * deltaTime;
		move[1] += fallSpeed * deltaTime;
	}

    // kontrola kolizí
	float checkPosX[3] = { pos[0] + move[0], pos[1], pos[2] };
    if (!checkPlayerCollision(checkPosX))
        pos[0] += move[0];

    float checkPosY[3] = { pos[0], pos[1] + move[1], pos[2] };
    grounded = checkPlayerCollision(checkPosY);
    if (!grounded)
        pos[1] += move[1];

    float checkPosZ[3] = { pos[0], pos[1], pos[2] + move[2] };
    if (!checkPlayerCollision(checkPosZ))
        pos[2] += move[2];

    // predani informace o poloze hrace zvukovemu enginu
	ALfloat listenerPos[] = { pos[0], pos[1], pos[2] };
	ALfloat listenerVel[] = { move[0], move[1], move[2] };
	ALfloat listenerOri[] = { delta[0], 0.0f, delta[1], 0.0f, 1.0f, 0.0f };
	soundSystem.setPlayerPosition(listenerPos, listenerVel, listenerOri);
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

void Player::staticMouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Player* player = static_cast<Player*>(glfwGetWindowUserPointer(window));
    if (player) {
        player->mouseButtonCallback(window, button, action, mods);
    }
}

void Player::staticWindowSizeCallback(GLFWwindow* window, int width, int height) {
    Player* player = static_cast<Player*>(glfwGetWindowUserPointer(window));
    if (player) {
        player->windowSizeCallback(window, width, height);
    }
}