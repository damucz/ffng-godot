#include "GodotSDL_mixer.h"
#include "core/math/math_funcs.h"
#include <string>

Mix_Channel Mix_channels[Mix_MaxChannels];

void Mix_HaltChannel(int channel) {
    Mix_FreeChannel(channel);
}

bool Mix_Playing(int channel) {
    DEV_ASSERT(channel < Mix_MaxChannels);
    return Mix_channels[channel].player->is_playing();
}

const Mix_Chunk* Mix_GetChunk(int channel) {
    DEV_ASSERT(channel < Mix_MaxChannels);
    return Mix_channels[channel].chunk;
}

void Mix_FreeChunk(Mix_Chunk* chunk) {
    for (int i = 0; i < Mix_MaxChannels; ++i) {
        if (Mix_channels[i].chunk == chunk) {
            Mix_FreeChannel(i);
            break;
        }
    }
    memdelete(chunk);
}

void Mix_FreeChannel(int channel) {
    Mix_Channel& ch = Mix_channels[channel];
    ch.player->stop();
    ch.player->set_stream(nullptr);
    ch.chunk = nullptr;
}

std::string Mix_GetError() {
    return "";
}

Mix_Chunk* Mix_LoadWAV(const char* filename) {
    Mix_Chunk* chunk = memnew(Mix_Chunk);
    Error error = OK;
    chunk->stream = ResourceLoader::load(filename, "", false, &error);
    DEV_ASSERT(error == OK);
    if (error != OK) {
		ERR_PRINT(vformat("Can't load sound: \"%s\"", String(filename)));
        chunk->stream = Ref<AudioStreamOGGVorbis>();
    }
    else {
        chunk->stream->set_loop(false);
    }
    return chunk;
}

int Mix_OpenAudio(int frequency, int format, int n2, int n1024) {
    for (int i = 0; i < Mix_MaxChannels; ++i) {
        auto& ch = Mix_channels[i];
        ch.player = memnew(AudioStreamPlayer);
        SDL_node->add_child(ch.player);
    }
    return 0;
}

void Mix_CloseAudio() {
    for (int i = 0; i < Mix_MaxChannels; ++i) {
        auto& ch = Mix_channels[i];
        SDL_node->remove_child(ch.player);
        ch.player->queue_delete();
    }
}

int Mix_PlayChannel(int channel, Mix_Chunk* sound, int loops) {
    DEV_ASSERT(loops == 0 || loops == -1);
    DEV_ASSERT(channel < Mix_MaxChannels);
    if (channel == -1) {
        // Find the first unused channel.
        for (int i = 0; i < Mix_MaxChannels; ++i) {
            if (!Mix_channels[i].player->is_playing()) {
                channel = i;
                break;
            }
        }
    }

    DEV_ASSERT(channel >= 0);
    auto &ch = Mix_channels[channel];
    // stop current chunk if any
    if (ch.player->is_playing())
        Mix_FreeChannel(channel);

    sound->stream->set_loop(loops == -1);
    ch.chunk = sound;
    ch.player->set_stream(sound->stream);
    ch.player->play();
    return channel;
}

void Mix_Volume(int channel, int volume) {
    float linear = (float)volume / MIX_MAX_VOLUME;

    if (channel == -1) {
        for (int i = 0; i < Mix_MaxChannels; ++i) {
            Mix_channels[i].player->set_volume_db(Math::linear2db(linear));
        }
    }
    else {
        Mix_channels[channel].player->set_volume_db(Math::linear2db(linear));
    }
}
