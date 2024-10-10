#include "audio_system.h"
#include <cstdio>

AudioSystem g_audioSystem;

// Initialize static members
uint16 AudioSystem::sound_buffer[SOUND_BUFFER_SIZE] = {0};
CSoundFile* AudioSystem::soundfile = nullptr;

AudioSystem::AudioSystem() : mod_buffer(nullptr), stream_handle(0) {
    // Initialize any non-static members here if needed
}

AudioSystem::~AudioSystem() {
    cleanup();
}

bool AudioSystem::initialize(const char* filename) {
    printf("Audio system initialization beginning\n");

    snd_stream_init();

    uint32 hnd = fs_open(filename, O_RDONLY);
    if (!hnd) {
        printf("Error reading %s\n", filename);
        return false;
    }

    printf("Filesize: %i\n", fs_total(hnd));
    mod_buffer = (uint8*)malloc(fs_total(hnd));

    if (!mod_buffer) {
        printf("Not enough memory\n");
        return false;
    }

    printf("Memory allocated\n");

    if ((size_t)fs_read(hnd, mod_buffer, fs_total(hnd)) != fs_total(hnd)) {
        printf("Read error\n");
        free(mod_buffer);
        return false;
    }

    printf("File read\n");

    soundfile = new CSoundFile;

    if (!soundfile) {
        printf("Not enough memory\n");
        free(mod_buffer);
        return false;
    }

    printf("CSoundFile created\n");

    if (!soundfile->Create(mod_buffer, fs_total(hnd))) {
        printf("Mod not loaded\n");
        free(mod_buffer);
        delete soundfile;
        return false;
    }

    printf("Mod loaded\n");
    soundfile->SetWaveConfig(44100, 16, 2);
    printf("Type: %li\n", soundfile->GetType());
    printf("Title: %s\n", soundfile->GetTitle());

    stream_handle = snd_stream_alloc(mod_callback, SND_STREAM_BUFFER_MAX);
    snd_stream_start(stream_handle, 44100, 1);

    return true;
}

void AudioSystem::update() {
    snd_stream_poll(stream_handle);
}

void AudioSystem::cleanup() {
    if (soundfile) {
        delete soundfile;
        soundfile = nullptr;
    }
    if (mod_buffer) {
        free(mod_buffer);
        mod_buffer = nullptr;
    }
    if (stream_handle) {
        snd_stream_stop(stream_handle);
        snd_stream_destroy(stream_handle);
        stream_handle = 0;
    }
    snd_stream_shutdown();
}

void* AudioSystem::mod_callback(snd_stream_hnd_t hnd, int len, int* actual) {
    int res;

    res = soundfile->Read(sound_buffer, len) * 4/*samplesize*/;

    //printf("res: %i, len: %i\n",res,len);
    if(res < len) {
        soundfile->SetCurrentPos(0);
        soundfile->Read(&sound_buffer[res], len - res);
    }

    *actual = len;

    return sound_buffer;
}