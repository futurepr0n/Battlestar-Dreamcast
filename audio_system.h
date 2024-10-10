#ifndef AUDIO_SYSTEM_H
#define AUDIO_SYSTEM_H

#include <kos.h>
#include <mp3/sndserver.h>
#include <modplug/stdafx.h>
#include <modplug/sndfile.h>

class AudioSystem {
public:
    AudioSystem();
    ~AudioSystem();

    bool initialize(const char* filename);
    void update();
    void cleanup();

private:
    static const int SOUND_BUFFER_SIZE = 65536;
    static uint16 sound_buffer[SOUND_BUFFER_SIZE];
    static CSoundFile* soundfile;

    uint8* mod_buffer;
    snd_stream_hnd_t stream_handle;

    static void* mod_callback(snd_stream_hnd_t hnd, int len, int* actual);
};

extern AudioSystem g_audioSystem;

#endif // AUDIO_SYSTEM_H