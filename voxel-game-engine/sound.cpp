#include "sound.h"
#include <iostream>
#include <fstream>

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

bool Sound::loadWavFile(const std::string& filename, ALuint& buffer)
{
    // Basic WAV file loading (simplified)
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open WAV file: " << filename << std::endl;
        return false;
    }

    char header[44];
    file.read(header, 44); // Read WAV header

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

ALuint Sound::createSource(ALuint buffer)
{
    ALuint source;
    alGenSources(1, &source);
    alSourcei(source, AL_BUFFER, buffer);
    sources.push_back(source);

    alSourcef(source, AL_ROLLOFF_FACTOR, 0.1f);

    return source;
}

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

void Sound::playSound(const std::string& filename, float volume)
{
    ALfloat listenerPos[3];
    alGetListenerfv(AL_POSITION, listenerPos);
    playSound(filename, listenerPos, volume);
}

void Sound::setPlayerPosition(ALfloat listenerPos[], ALfloat listenerVel[], ALfloat listenerOri[])
{
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);
}