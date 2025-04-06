#pragma once

#include <AL/al.h>
#include <AL/alc.h>
#include <string>
#include <vector>

class Sound
{
public:
    /**
     * @brief Constructor for the Sound class.
     */
    Sound();
    
    /**
     * @brief Destructor for the Sound class, cleanup.
     */
    ~Sound();

    /**
     * @brief Play a sound from a WAV file at the specified position and optionally volume.
     *
     * @param filename The name of the WAV file to play.
     * @param sourcePos The position of the sound source within the game world.
     * @param volume The volume of the sound from 0.0 (mute) to 1.0 (max). Defaults to 1.0.
     */
    void playSound(const std::string& filename, ALfloat sourcePos[], float volume = 1.0f);
    
    /**
     * @brief Play a sound from a WAV file at the position of the player and optionally at a specified volume.
     *
     * @param filename The name of the WAV file to play.
     * @param volume The volume of the sound from 0.0 (mute) to 1.0 (max). Defaults to 1.0.
     */
    void playSound(const std::string& filename, float volume = 1.0f);
    
    /**
     * @brief Set the position and velocity of the listener (player).
     *
     * @param listenerPos The position of the listener in the game world.
     * @param listenerVel The velocity of the listener.
     * @param listenerOri The orientation of the listener.
     */
    void setPlayerPosition(ALfloat listenerPos[], ALfloat listenerVel[], ALfloat listenerOri[]);

private:
    ALCdevice* device;
    ALCcontext* context;
    std::vector<ALuint> buffers;
    std::vector<ALuint> sources;

    /**
     * @brief Load a WAV file and create an OpenAL buffer.
     *
     * @param filename The name of the WAV file to load.
     * @param buffer The OpenAL buffer to fill with audio data.
     * @return True if successful, false otherwise.
     */
    bool loadWavFile(const std::string& filename, ALuint& buffer);
    
    /**
     * @brief Create an OpenAL source and attach a buffer to it.
     *
     * @param buffer The OpenAL buffer to attach to the source.
     * @return The OpenAL source ID.
     */
    ALuint createSource(ALuint buffer);
};