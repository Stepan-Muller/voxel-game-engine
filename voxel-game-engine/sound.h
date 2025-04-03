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

    bool loadWavFile(const std::string& filename, ALuint& buffer);
    ALuint createSource(ALuint buffer);
    void playSound(ALuint source),
         stopSound(ALuint source);

private:
    ALCdevice* device;
    ALCcontext* context;
    std::vector<ALuint> buffers,
                        sources;
};