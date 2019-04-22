#include "metadata.h"
#include "main.h"
#include "compositor.h"
#include "toolkit.h"
#include "threads.h"

#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

void frameCompositor (threadsPPC_t *ppc) {

    PosixThreadIPC_t *properties = ppc->prop;
    Metadata_t *metadata = ppc->md;
    unsigned long int frameStartOffset = properties->frameOffset;

    properties->compositorBreak = false;

    // 定位到第一帧头部
    lseek(properties->meltFileDescriptor, frameStartOffset, SEEK_SET);

    properties->buffer = malloc(2);
    properties->bigBuffer = malloc(metadata->frameSize - 2);
    properties->frameBuffer = malloc(metadata->frameSize);
    properties->frameBuffer_Back = malloc(metadata->frameSize);

    long int sizeCounter = 0;
    struct stat stbuf;
    fstat(properties->meltFileDescriptor, &stbuf);

    while (sizeCounter < stbuf.st_size) {
        // 由 inputProcesser 线程发送的混成暂停信号
        while (properties->compositorPause)
            usleep(properties->compositorTriggerTime);
        if (properties->compositorBreak) break;


        // 首先读取两个字节确定是帧字节还是控制符
        read(properties->meltFileDescriptor, properties->buffer, 2);
        // 清屏控制符
        if (properties->buffer[0] == '\0' && properties->buffer[1] == '\n') {
            write(STDOUT_FILENO, "\033[3J", 4);
            sizeCounter += 2;
            continue;
        }

        memset(properties->frameBuffer, '\0', metadata->frameSize);
        memset(properties->bigBuffer, '\0', metadata->frameSize - 2);
        read(properties->meltFileDescriptor, properties->bigBuffer, metadata->frameSize - 2);

        sizeCounter += metadata->frameSize;

        // 若待合成帧与上一帧相似度大于设定值，触发跳帧功能
        properties->sameRate = compareStr(properties->frameBuffer_Back, properties->bigBuffer, metadata->frameSize - 2, 2);
        if (properties->sameRate >= properties->args.skippingFrameRate && sizeCounter < stbuf.st_size) {
            properties->skipedFrames++;
            // 保存当前帧到备用缓冲区
            // strcpy(properties->frameBuffer_Back, properties->frameBuffer);
            usleep((int)properties->args.timeSpeed);
            continue;
        }

        // 渲染帧
        strcpy(properties->frameBuffer, properties->buffer);
        strcat(properties->frameBuffer, properties->bigBuffer);
        printf (
            "\033[32;1mCompositor.SkipedFrameCount: %d\n\
Compositor.FramesSame rate: %f\n\
Compositor.WritedSize: %ld / %ld\n\
Compositor.TimeSpeed: %f\033[0m\n",
            properties->skipedFrames,
            properties->sameRate,
            sizeCounter,
            stbuf.st_size,
            properties->args.timeSpeed
            // metadata->RGBOption
        );
        printf (
            "\033[34;1m\
Sound.Channels: %d\n\
Sound.Rate: %d\n\
Sound.WritedSize: %ld / %ld\n\
Sound.Status: %s\033[0m",
            properties->sound.channel,
            properties->sound.rate,
            properties->sound.writed, properties->sound.datalen,
            properties->sound.audioStatus
        );

        puts(metadata->RGBOption);
        puts(metadata->highLight);
        write(STDOUT_FILENO, properties->frameBuffer, metadata->frameSize);
        printf("\033[0m");

        // 保存当前帧到备用缓冲区
        strcpy(properties->frameBuffer_Back, properties->frameBuffer);

        usleep((int)properties->args.timeSpeed);
    }
}
