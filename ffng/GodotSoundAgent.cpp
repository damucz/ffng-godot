#include "GodotSoundAgent.h"
#include "Path.h"
#include "core/os/file_access.h"

#include "Log.h"
#include "ExInfo.h"
#include "SDLException.h"
#include "MixException.h"
#include "Random.h"
#include "BaseMsg.h"
#include "OptionAgent.h"

void GodotSoundAgent::own_init() {
    Mix_OpenAudio(22050, AUDIO_U8, 2, 1024);
    SoundAgent::own_init();
}

void GodotSoundAgent::own_shutdown() {
    stopMusic();
    Mix_CloseAudio();
}

/**
 * Play this sound.
 * @param sound chunk to play
 * @param volume percentage sound volume
 * @param loops numer of loops. 0=play once, 1=play twice, -1=play infinite
 *
 * @return channel number where the sound is played,
 * return -1 on error or when sound is NULL
 */
    int
GodotSoundAgent::playSound(Mix_Chunk *sound, int volume, int loops)
{
    int channel = -1;
    if (sound) {
        DEV_ASSERT(sound);
        // loops should be always 0 (play once) or -1 (play infinetely)
        DEV_ASSERT(loops == 0 || loops == -1);

        channel = Mix_PlayChannel(-1, sound, loops);
        if (-1 == channel) {
            //NOTE: maybe there are too few open channels
            LOG_WARNING(ExInfo("cannot play sound")
                    .addInfo("Mix", Mix_GetError()));
        }
        else {
            Mix_Volume(channel, m_soundVolume * volume / 100);
        }
    }
    return channel;
}

//-----------------------------------------------------------------
/**
 * Set sound volume.
 * NOTE: all already running sound will get equals volume
 * @param volume percentage volume, e.g. 30%=30
 */
    void
GodotSoundAgent::setSoundVolume(int volume)
{
    m_soundVolume = MIX_MAX_VOLUME * volume / 100;
    if (m_soundVolume > MIX_MAX_VOLUME) {
        m_soundVolume = MIX_MAX_VOLUME;
    }
    else if (m_soundVolume < 0) {
        m_soundVolume = 0;
    }
    Mix_Volume(-1, m_soundVolume);
    if (m_musicChannel != -1)
        Mix_Volume(m_musicChannel, m_musicVolume);
}

//---------------------------------------------------------------------------
// Music part
//---------------------------------------------------------------------------

/**
 * Play music.
 * @param file path to music file (i.e. *.ogg)
 * @param finished send this message when music is finished.
 * If finished is NULL, play music forever.
 */
void
GodotSoundAgent::playMusic(const Path &file,
        BaseMsg *finished)
{
    DEV_ASSERT(finished == nullptr);

    // The same music is not restarted when it is not needed.
    if (m_playingPath == file.getPosixName()) {
        return;
    }

    stopMusic();
    m_playingPath = file.getPosixName();

    m_music = Mix_LoadWAV(file.getNative().c_str());
    m_musicChannel = Mix_PlayChannel(-1, m_music, -1);
    Mix_Volume(m_musicChannel, m_musicVolume);

    // read loop offset from meta file
    Error error;
    String s = FileAccess::get_file_as_string((file.getNative() + ".meta").c_str(), &error);
    if (error == OK) {
        Vector<String> lines = s.split("\n");
        if (!lines.empty()) {
            float offset = lines[0].to_int() / 22050.0f;
            m_music->stream->set_loop_offset(offset);         // menu = 19.03727891156463f
        }
    }
}
//-----------------------------------------------------------------
/**
 * @param volume percentage volume, e.g. 30%=30
 */
    void
GodotSoundAgent::setMusicVolume(int volume)
{
    m_musicVolume = MIX_MAX_VOLUME * volume / 100;
    if (m_musicVolume > MIX_MAX_VOLUME) {
        m_musicVolume = MIX_MAX_VOLUME;
    }
    else if (m_musicVolume < 0) {
        m_musicVolume = 0;
    }
    Mix_Volume(m_musicChannel, m_musicVolume);
}
//-----------------------------------------------------------------
    void
GodotSoundAgent::stopMusic()
{
    if (m_music) {
        Mix_FreeChunk(m_music);
        m_music = nullptr;
    }
    m_playingPath = "";
}

