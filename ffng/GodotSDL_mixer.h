#ifndef GODOTSDL_MIXER_HEADER
#define GODOTSDL_MIXER_HEADER

#include "GodotSDL.h"
#include "core/dictionary.h"
//#include "servers/audio/audio_stream.h"
#include "modules/stb_vorbis/audio_stream_ogg_vorbis.h"
#include "scene/audio/audio_stream_player.h"

#define MIX_MAX_VOLUME 100
#define AUDIO_S8 0
#define AUDIO_U8 1
#define MIX_DEFAULT_FORMAT 0

class Mix_Chunk
{
public:
    Ref<AudioStreamOGGVorbis> stream;
};

class Mix_Channel {
public:
    Mix_Chunk* chunk = nullptr;
    AudioStreamPlayer* player = nullptr;
};

constexpr int Mix_MaxChannels = 64;
extern Mix_Channel Mix_channels[Mix_MaxChannels];

void Mix_HaltChannel(int channel);
bool Mix_Playing(int channel);
const Mix_Chunk* Mix_GetChunk(int channel);
void Mix_FreeChunk(Mix_Chunk* chunk);
void Mix_FreeChannel(int channel);
std::string Mix_GetError();
Mix_Chunk* Mix_LoadWAV(const char* filename);
int Mix_OpenAudio(int frequency, int format, int n2, int n1024);
void Mix_CloseAudio();
int Mix_PlayChannel(int channel, Mix_Chunk* sound, int loops);
void Mix_Volume(int channel, int volume);

#endif
