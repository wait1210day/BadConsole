#ifndef _MAIN_H
#define _MAIN_H 1

#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>

#include "metadata.h"

#ifdef __gnu_linux__
#define CCB_enable_pulseaudio   1
#include <pulse/simple.h>
#endif

// 此结构体保存命令行传入的参数
typedef struct _Settings_Struct {
    double timeSpeed;
    double skippingFrameRate;
    char *meltedFile;
    char *wavFile;
    char *pulseLib;

} CommandLineARGS_t;

// 声音子系统
typedef struct _Sound_Subsystem_Struct {
#ifdef __gnu_linux__
    pa_simple * (*pa_simple_new) (
        const char *,
        const char *,
        pa_stream_direction_t,
        const char *,
        const char *,
        const pa_sample_spec *,
        const pa_channel_map *,
        const pa_buffer_attr *,
        int *
    );

    int (* pa_simple_write) (
        pa_simple *,
        const void *,
        size_t,
        int *
    );

    int (*pa_simple_drain) (
        pa_simple *,
        int *
    );

    void (*pa_simple_free) (
        pa_simple *
    );
#endif

    void *pulseaudio;

    uint8_t *audioWAVData;
    char audioStatus[100];
    size_t datalen;

    uint32_t rate;
    uint8_t channel;

    size_t writed;

    bool soundPause;
    int soundTriggerTime;

    bool soundBreak;
} Soundsys_t;

// 命令行子系统
typedef struct _Commandline_Properties_Struct {
    bool cmdPause;
    int cmdTriggerTime;
} Promptsys_t;

// 此结构体沟通所有关键函数并传递参数
typedef struct _posixThread_IPC_Struct {
    /* 终止而非暂停混成器 */
    bool compositorBreak;

    /* inputProcess 线程结束通知 */
    bool toBeContinue;

    /* 混成器暂停通知以及等待再次启动的间隔时间 */
    bool compositorPause;
    int compositorTriggerTime;

    /* 播放时间步长 */
    double timeSpeed;

    /* 混成器缓冲区 */
    char *buffer;
    char *bigBuffer;
    char *frameBuffer;
    /**
     * 此缓冲区保存上一帧的内容，在绘制当前帧之前，
     * 混成器比较将要绘制的帧和上一帧的相似度，如果
     * 相似度高于一定数值（通常由用户设置），则触发跳帧
     * 功能：丢弃当前帧，保留旧的缓冲区，但是仍会等待
     * 帧时间步长
    */
    char *frameBuffer_Back;

    /* 当前帧与上一帧的相似度 */
    double sameRate;

    /* 已跳过的帧计数 */
    unsigned int skipedFrames;

    /* 第一帧文件偏移量 */
    unsigned long int frameOffset;

    /* 打开的帧描述文件标识符（文件描述符） */
    int meltFileDescriptor;

    CommandLineARGS_t args;
    Soundsys_t sound;
    Promptsys_t prompt;

} PosixThreadIPC_t;

/**
 * 可以看做 PosuxThreadIPC_t 和 Metadata_t 结构体的整合
*/
typedef struct _PPC_Start {
    PosixThreadIPC_t *prop;
    Metadata_t *md;
} threadsPPC_t;

#endif // _MAIN_H