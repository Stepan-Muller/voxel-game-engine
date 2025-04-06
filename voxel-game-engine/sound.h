#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <vector>

class Sound
{
public:
    Sound();
    ~Sound();

    void playSound(const std::string& filename, ALfloat sourcePos[], float volume = 1.0f),
         playSound(const std::string& filename, float volume = 1.0f),
         setPlayerPosition(ALfloat listenerPos[], ALfloat listenerVel[], ALfloat listenerOri[]);

private:
    ALCdevice* device;
    ALCcontext* context;
    std::vector<ALuint> buffers,
                        sources;

    bool loadWavFile(const std::string& filename, ALuint& buffer);
    ALuint createSource(ALuint buffer);
};