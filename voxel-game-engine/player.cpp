#pragma once

#include "player.h"

Player::Player(Map* _map) {
	map = *_map;
    
    std::string screenVertexSource = loadShaderSource("vertex.glsl");
    const GLchar* screenVertexShaderSource = screenVertexSource.c_str();
    
    std::string screenFragmentSource = loadShaderSource("fragment.glsl");
    const GLchar* screenFragmentShaderSource = screenFragmentSource.c_str();
    
    std::string screenComputeSource = loadShaderSource("compute.glsl");
    const GLchar* screenComputeShaderSource = screenComputeSource.c_str();

    GLuint screenTex;
    
    // screen geometry
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

    // OpenGL version selection
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OPENGL_MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OPENGL_MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window creation
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Voxel Game Engine", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create the GLFW window\n";
        glfwTerminate();
    }
    glfwSetWindowUserPointer(window, this);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(vSync);

    // setting GLFW callbacks
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

    // bind geometry into the buffers
    glNamedBufferData(VBO, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glNamedBufferData(EBO, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnableVertexArrayAttrib(VAO, 0);
    glVertexArrayAttribBinding(VAO, 0, 0);
    glVertexArrayAttribFormat(VAO, 0, 3, GL_FLOAT, GL_FALSE, 0);

    glEnableVertexArrayAttrib(VAO, 1);
    glVertexArrayAttribBinding(VAO, 1, 0);
    glVertexArrayAttribFormat(VAO, 1, 2, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat));

    // binding VBO and EBO to VAO
    glVertexArrayVertexBuffer(VAO, 0, VBO, 0, 5 * sizeof(GLfloat));
    glVertexArrayElementBuffer(VAO, EBO);

    // texture setting
    glCreateTextures(GL_TEXTURE_2D, 1, &screenTex);
    glTextureParameteri(screenTex, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(screenTex, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTextureParameteri(screenTex, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, screenTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, screenWidth, screenHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
    glBindImageTexture(0, screenTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	// buffer for the voxel in the middle of the screen
    glGenBuffers(1, &hitBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, hitBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, 6 * sizeof(int), NULL, GL_DYNAMIC_READ);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, hitBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // screen shaders
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

	// gui initialization   
	gui = new Gui(window);

    // delta time setup
    float lastTime = (float)glfwGetTime();

    loadMeta(&map, L"demo");
    respawn();

    /* Main game loop */
    while (!glfwWindowShouldClose(window))
    {
        // delta time calculation
        deltaTime = (float)glfwGetTime() - lastTime;
        lastTime = (float)glfwGetTime();

        // player movement
        movePlayer(window);

        // chunk update
        int chunkPos[2] = {floor(pos[0] / (&map)->chunkWidth), floor(pos[2] / (&map)->chunkWidth)};
        if (chunkPos[0] != lastChunkPos[0] || chunkPos[1] != lastChunkPos[1])
        {
            lastChunkPos[0] = chunkPos[0];
            lastChunkPos[1] = chunkPos[1];
            (&map)->updateChunks(chunkPos, renderDistance);
        }

        // callbacks
        glfwPollEvents();

        // compute shader
        glUseProgram(computeProgram);
        // compute shaderu parameters
        glUniform1i(glGetUniformLocation(computeProgram, "renderDist"), renderDistance);
		glUniform1i(glGetUniformLocation(computeProgram, "chunkWidth"), (&map)->chunkWidth);
        glUniform2f(glGetUniformLocation(computeProgram, "angle"), angle[0], angle[1]);
        glUniform1f(glGetUniformLocation(computeProgram, "fov"), fov);
        glUniform3f(glGetUniformLocation(computeProgram, "playerPos"), pos[0], pos[1], pos[2]);
        glUniform3f(glGetUniformLocation(computeProgram, "sunDir"), (&map)->sunDir[0], (&map)->sunDir[1], (&map)->sunDir[2]);
        glUniform3f(glGetUniformLocation(computeProgram, "skyColor"), (&map)->skyColor[0], (&map)->skyColor[1], (&map)->skyColor[2]);

        glDispatchCompute(screenWidth / 8 + 1, screenHeight / 4 + 1, 1);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);

        // screen shader
        glUseProgram(screenShaderProgram);
        // screen shaderu parameters
        glBindTextureUnit(0, screenTex);
        glUniform1i(glGetUniformLocation(screenShaderProgram, "screen"), 0);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]), GL_UNSIGNED_INT, 0);

		// menu gui rendering
		if (menu) gui->render();

        glfwSwapBuffers(window);
    }

    // glfw cleanup
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
	pos[0] = (&map)->spawnPos[0];
	pos[1] = (&map)->spawnPos[1];
	pos[2] = (&map)->spawnPos[2];
	angle[0] = (&map)->spawnAngle[0];
	angle[1] = (&map)->spawnAngle[1];
}

void Player::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // release the mouse in the menu
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        menu = !menu;
        if (menu)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        resetMouse = true;
    }

    // map save
    if (menu && key == GLFW_KEY_S && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        saveMeta(&map);
	    (&map)->saveChunks();
    }

    // map load
    if (menu && key == GLFW_KEY_O && action == GLFW_PRESS && glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        loadMeta(&map);
        respawn();
    }

    // respawn
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
        respawn();
}

void Player::mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    // do not move the camera while in the menu
    if (menu) return;

    if (resetMouse)
    {
        lastMouse[0] = (float)xpos;
        lastMouse[1] = (float)ypos;
        resetMouse = false;
    }

    // x axis - player rotation
    angle[0] = capRad360(angle[0] + ((float)xpos - lastMouse[0]) * turnSpeed);
    lastMouse[0] = (float)xpos;

    // y axis - camera rotation
    angle[1] += capRad90_90(((float)ypos - lastMouse[1]) * turnSpeed);
    lastMouse[1] = (float)ypos;
}

void Player::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    // do not place voxels while in the menu
    if (menu) return;
    
    // voxel placement
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

        if (hitNormal[0] == 0 && hitNormal[1] == 0 && hitNormal[2] == 0)
            return;

		int hitNeighbour[3] = { hitVoxel[0] - hitNormal[0], hitVoxel[1] - hitNormal[1], hitVoxel[2] - hitNormal[2] };

		(&map)->changeVoxel(hitNeighbour, gui->selectedVoxel, gui->selectedVoxelCollision);
        (&map)->updateChunks(lastChunkPos, renderDistance);
	}
    
	// voxel removal
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
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

		if (hitNormal[0] == 0 && hitNormal[1] == 0 && hitNormal[2] == 0)
			return;

        (&map)->changeVoxel(hitVoxel, new float[5] { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f }, false);
        (&map)->updateChunks(lastChunkPos, renderDistance);
    }
}

void Player::windowSizeCallback(GLFWwindow* window, int width, int height)
{
    screenWidth = width;
    screenHeight = height;

    glViewport(0, 0, screenWidth, screenHeight);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
}

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
                
                if ((&map)->checkCollision(voxelPos))
                    return true;
            }
        }
    }

    return false;
}

void Player::movePlayer(GLFWwindow* window)
{
	// do not move the player while in the menu
    if (menu) return;

    int speedMultiplier = glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) ? 1 : 2;

    float move[3] = {0, 0, 0};

    float delta[2];
    delta[0] = sin(angle[0]);
    delta[1] = cos(angle[0]);

    if (glfwGetKey(window, GLFW_KEY_W))
    {
        move[0] -= delta[0] * deltaTime * (&map)->moveSpeed * speedMultiplier;
        move[2] -= delta[1] * deltaTime * (&map)->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_S))
    {
        move[0] += delta[0] * deltaTime * (&map)->moveSpeed * speedMultiplier;
        move[2] += delta[1] * deltaTime * (&map)->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_A))
    {
        move[0] += delta[1] * deltaTime * (&map)->moveSpeed * speedMultiplier;
        move[2] -= delta[0] * deltaTime * (&map)->moveSpeed * speedMultiplier;
    }

    if (glfwGetKey(window, GLFW_KEY_D))
    {
        move[0] -= delta[1] * deltaTime * (&map)->moveSpeed * speedMultiplier;
        move[2] += delta[0] * deltaTime * (&map)->moveSpeed * speedMultiplier;
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

	// collision detection
	float checkPosX[3] = { pos[0] + move[0], pos[1], pos[2] };
    if (checkPlayerCollision(checkPosX))
        move[0] = 0.0f;

    float checkPosY[3] = { pos[0], pos[1] + move[1], pos[2] };
    grounded = checkPlayerCollision(checkPosY);
    if (grounded)
        move[1] = 0.0f;

    float checkPosZ[3] = { pos[0], pos[1], pos[2] + move[2] };
    if (checkPlayerCollision(checkPosZ))
        move[2] = 0.0f;

    pos[0] += move[0];
    pos[1] += move[1];
    pos[2] += move[2];

    // footstep sounds
	stepTimer -= deltaTime;
    
    if (grounded && (move[0] || move[2]) && stepTimer <= 0.0f)
    {
		sound.playSound("sounds/step" + std::to_string(rand() % 6) + ".wav", 0.1f);
		stepTimer = STEP_COOLDOWN;
    }

    // falling sounds (louder footstep)
    if (grounded && fallSpeed >= 20)
        sound.playSound("sounds/step" + std::to_string(rand() % 6) + ".wav");
        

	// give the sound engine the player position information
	ALfloat listenerPos[] = { pos[0], pos[1], pos[2] };
	ALfloat listenerVel[] = { move[0], move[1], move[2] };
	ALfloat listenerOri[] = { delta[0], 0.0f, delta[1], 0.0f, 1.0f, 0.0f };
	sound.setPlayerPosition(listenerPos, listenerVel, listenerOri);
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