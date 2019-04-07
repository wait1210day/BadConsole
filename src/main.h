#ifndef _MAIN_H
#define _MAIN_H 1

#include <stdbool.h>
#include <sys/types.h>
#include <stdint.h>

// 此结构体保存命令行传入的参数
typedef struct _Settings_Struct {
    double timeSpeed;
    double skippingFrameRate;
    char const *meltedFile;

} CommandLineARGS_t;

// 声音子系统
typedef struct _Sound_Subsystem_Struct {
    uint8_t *audioWAVData;
    char audioStatus[100];
    size_t datalen;

    uint32_t rate;
    uint8_t channel;

    size_t writed;

    bool soundPause;
    int soundTriggerTime;

    bool soundBreak;
} soundSubsys_t;

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

    /* 打开的帧描述文件标识符（文件描述符） */
    int meltFileDescriptor;

    CommandLineARGS_t args;
    soundSubsys_t sound;

} PosixThreadIPC_t;

#endif // _MAIN_H