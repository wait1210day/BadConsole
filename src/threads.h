#ifndef _THREADS_H
#define _THREADS_H 1

#include <pthread.h>
#include <stdbool.h>
#include "metadata.h"
#include "main.h"

#define THR_BUFSIZE 1024

typedef struct _command_linked_list_struct {
    char *base;
    int len;
    struct _command_linked_list_struct *next;
} CmdLinkedList_t;

/**
 * 可以看做 PosuxThreadIPC_t 和 Metadata_t 结构体的整合
*/
typedef struct _PPC_Start {
    PosixThreadIPC_t *prop;
    Metadata_t *md;
} threadsPPC_t;

/**
 * 创建命令提示符服务线程。默认入口函数是 commandPrompt。
 * 此线程一旦被创建，将立刻读取控制台标准输入。线程只监听回车事件，
 * 多余的输入会被忽略并写入 ignoredInput 缓冲区中。一旦监听到回车事件，
 * 线程将立刻通知 Compositor 停止混成（通过 PosixThreadIPC_t 结构体
 * 实现），并调用 commandlineProcessService 函数。调用结束后，
 * 线程通知 Compositor 重新载入配置并继续混成。
*/
int createCommandlinePosixThread(pthread_t *id, threadsPPC_t *ppc);

/**
 * 此函数创建音频轨道播放器。
*/
int createPlaybackPosixThread(pthread_t *id, threadsPPC_t *ppc, int channels, int rate);
void WavDataPlayer(threadsPPC_t *ppc);
int loadWAVDataFromPath(char const *file, threadsPPC_t *ppc);

/**
 * 用于处理用户输入。其核心为
 * GNU Readline 库，通过响应用户输入，程序改变 PosixThreadIPC_t
 * 和 Metadata_t 结构体的数据，当混成器重新载入数据时，这些更改将被
 * 应用。
*/
void commandPrompt(threadsPPC_t *ppc);

#endif