#include "sound.h"
#include <iostream>
#include <fstream>

/**
 * @brief Constructor for the Sound class.
 */
Sound::Sound()
{
    device = alcOpenDevice(nullptr);
    if (!device) {
        std::cerr << "Failed to open OpenAL device!" << std::endl;
        return;
    }

    context = alcCreateContext(device, nullptr);
    if (!context || !alcMakeContextCurrent(context)) {
        std::cerr << "Failed to create or make OpenAL context current!" << std::endl;
        return;
    }
}

/**
 * @brief Destructor for the Sound class, cleanup.
 */
Sound::~Sound()
{
    for (ALuint source : sources) {
        alDeleteSources(1, &source);
    }

    for (ALuint buffer : buffers) {
        alDeleteBuffers(1, &buffer);
    }

    alcMakeContextCurrent(nullptr);

    if (context) 
        alcDestroyContext(context);

    if (device) 
        alcCloseDevice(device);
}

/**
 * @brief Load a WAV file and create an OpenAL buffer.
 *
 * @param filename The name of the WAV file to load.
 * @param buffer The OpenAL buffer to fill with audio data.
 * @return True if successful, false otherwise.
 */
bool Sound::loadWavFile(const std::string& filename, ALuint& buffer)
{
    // basic WAV file loading
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open WAV file: " << filename << std::endl;
        return false;
    }

    char header[44];
    file.read(header, 44); // read WAV header

    int dataSize = *reinterpret_cast<int*>(&header[40]);
    std::vector<char> data(dataSize);
    file.read(data.data(), dataSize);

    ALenum format = (header[22] == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;
    ALuint freq = *reinterpret_cast<int*>(&header[24]);

    alGenBuffers(1, &buffer);
    alBufferData(buffer, format, data.data(), dataSize, freq);
    buffers.push_back(buffer);

    return true;
}

/**
 * @brief Create an OpenAL source and attach a buffer to it.
 *
 * @param buffer The OpenAL buffer to attach to the source.
 * @return The OpenAL source ID.
 */
ALuint Sound::createSource(ALuint buffer)
{
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    sources.push_back(source);

    alSourcef(source, AL_ROLLOFF_FACTOR, 0.1f);

    return source;
}

/**
 * @brief Play a sound from a WAV file at the specified position and optionally volume.
 *
 * @param filename The name of the WAV file to play.
 * @param sourcePos The position of the sound source within the game world.
 * @param volume The volume of the sound from 0.0 (mute) to 1.0 (max). Defaults to 1.0.
 */
void Sound::playSound(const std::string& filename, ALfloat sourcePos[], float volume)
{
    ALuint buffer;
    if (loadWavFile(filename, buffer)) {
        ALuint source = createSource(buffer);
        alSourcefv(source, AL_POSITION, sourcePos);
        alSourcef(source, AL_GAIN, volume);
        alSourcePlay(source);
    }
}

/**
 * @brief Play a sound from a WAV file at the position of the player and optionally at a specified volume.
 *
 * @param filename The name of the WAV file to play.
 * @param volume The volume of the sound from 0.0 (mute) to 1.0 (max). Defaults to 1.0.
 */
void Sound::playSound(const std::string& filename, float volume)
{
    ALfloat listenerPos[3];
    alGetListenerfv(AL_POSITION, listenerPos);
    playSound(filename, listenerPos, volume);
}

/**
 * @brief Set the position and velocity of the listener (player).
 *
 * @param listenerPos The position of the listener in the game world.
 * @param listenerVel The velocity of the listener.
 * @param listenerOri The orientation of the listener.
 */
void Sound::setPlayerPosition(ALfloat listenerPos[], ALfloat listenerVel[], ALfloat listenerOri[])
{
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
}