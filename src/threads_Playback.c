/**
 * 本文件负责播放音频流。作为音频服务线程的入口函数。
 * 播放后端使用 PulseAudio
*/

#include "threads.h"
#include "main.h"
#include "compositor.h"
#include <pthread.h>

#ifdef __gnu_linux__
#include <pulse/simple.h>
#include <pulse/error.h>
#endif

#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
// #include <stdlib.h>

void WavDataPlayer(threadsPPC_t *ppc) {
#ifdef __gnu_linux__
    pa_simple *sound = NULL;
    ppc->prop->sound.writed = 0;
    ppc->prop->sound.soundBreak = false;
    int error;
    // size_t r;
    // pa_usec_t latency;

    // 报告声音子系统状态
    strcpy(ppc->prop->sound.audioStatus, "Starting");
    
    static pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = ppc->prop->sound.rate;
    ss.channels = ppc->prop->sound.channel;

    sound = ppc->prop->sound.pa_simple_new (
        NULL,
        "MelterSoundService",
        PA_STREAM_PLAYBACK,
        NULL,
        "playback",
        &ss,
        NULL,
        NULL,
        &error
    );

    if (!sound) {
        strcpy(ppc->prop->sound.audioStatus, "pa_simple_new() failed");
        return ;
    }

    uint8_t buf[1024];
    for (size_t i = 0; i <= ppc->prop->sound.datalen; ) {
        while (ppc->prop->sound.soundPause)
            usleep(ppc->prop->sound.soundTriggerTime);
        if (ppc->prop->sound.soundBreak) break;

        strcpy(ppc->prop->sound.audioStatus, "Copying buffer");
        for (size_t j = 0; j != 1024; j++, i++) {
            buf[j] = ppc->prop->sound.audioWAVData[i];
        }
        strcpy(ppc->prop->sound.audioStatus, "PulseAudio");
        if (ppc->prop->sound.pa_simple_write(sound, buf, 1024, &error)) {
            strcpy(ppc->prop->sound.audioStatus, "pa_simple_write() failed");
            return ;
        }
        ppc->prop->sound.writed += 1024;
    }

    ppc->prop->sound.pa_simple_drain(sound, &error);
    ppc->prop->sound.pa_simple_free(sound);
#else
    strcpy(ppc->prop->sound.audioStatus, "Not Supported");
#endif
}

int loadWAVDataFromPath(threadsPPC_t *ppc) {
    char *file = ppc->prop->args.wavFile;

    int fd = open(file, O_RDONLY);
    if (fd < 0) {
        perror("Melter: loadWAVDataFromPath: Cannot open file");
        return -1;
    }

    struct stat stbuf;
    fstat(fd, &stbuf);
    ppc->prop->sound.datalen = stbuf.st_size;
    ppc->prop->sound.audioWAVData = malloc(stbuf.st_size);

    read(fd, ppc->prop->sound.audioWAVData, stbuf.st_size);

    close(fd);
    return 0;
}

int createPlaybackPosixThread (pthread_t *id, threadsPPC_t *ppc, int channels, int rate) {
    ppc->prop->sound.rate = rate;
    ppc->prop->sound.channel = channels;
    ppc->prop->sound.soundPause = true;
    ppc->prop->sound.soundTriggerTime = 1000;

    if (pthread_create(id, NULL, (void *)WavDataPlayer, ppc) < 0) {
        printf("Melter: Cannot create player thread\n");
        return -1;
    }
    return 0;
}

int createCompositionPosixThread (pthread_t *id, threadsPPC_t *ppc, unsigned long int off) {
    ppc->prop->frameOffset = off;
    ppc->prop->compositorTriggerTime = (int)ppc->prop->args.timeSpeed * 10;

    if (pthread_create (id, NULL, (void *) frameCompositor, ppc) < 0) {
        printf ("Melter: Cannot create composition thread\n");
        return -1;
    }

    return 0;
}
