#ifndef HEADER_GODOTSOUNDAGENT_H
#define HEADER_GODOTSOUNDAGENT_H

#include "SoundAgent.h"
#include <string>

/**
 * Sound and music.
 */
class GodotSoundAgent : public SoundAgent {
    private:
    std::string m_playingPath;
    Mix_Chunk* m_music = nullptr;
    int m_musicChannel = -1;
    int m_soundVolume = 0;
    int m_musicVolume = 0;

    private:
        //std::string generateIdName(const Path &file);
        //Mix_Chunk *findChunk(const std::string &name);

        //static void musicFinished();
    protected:
        virtual void own_init();
        virtual void own_shutdown();
        //virtual void reinit();

        virtual void setSoundVolume(int volume);
        virtual void setMusicVolume(int volume);
    public:
        virtual int playSound(Mix_Chunk *sound, int volume, int loops=0);

        virtual void playMusic(const Path &file,
                BaseMsg *finished);
        virtual void stopMusic();
};

#endif
