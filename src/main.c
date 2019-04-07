#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <malloc.h>
#include <unistd.h>
#include <pthread.h>


#include "metadata.h"
#include "main.h"
#include "compositor.h"
#include "toolkit.h"
#include "threads.h"
#include "subcmds.h"

int main(int argc, char const *argv[]) {
    Metadata_t fileMetaData;
    PosixThreadIPC_t property;
    threadsPPC_t ppc;

    pthread_t playbackThread;
    pthread_t commandline;

    property.buffer = NULL;
    property.bigBuffer = NULL;
    property.frameBuffer = NULL;
    property.frameBuffer_Back = NULL;
    property.sound.soundBreak = false;
    property.compositorBreak = false;
    ppc.md = &fileMetaData;
    ppc.prop = &property;


    // 加载 WAV 文件并创建播放线程
    loadWAVDataFromPath (argv[4], &ppc);
    createPlaybackPosixThread (&playbackThread, &ppc, 2, 44100);

    // 初始化结构体并打开相应文件
    if (initDataStruct(&property, argv, argc) < 0) return 1;
    // 解析文件头数据
    if (parseMeltedFileHeader(&property, &fileMetaData) < 0) return 1;
    printMetadata(&fileMetaData);

    // 等待确认输入
    printf("\033[31;1m[!] Press Enter key to start compositor...\033[0m\n");
    getc(stdin);

    // 创建交互式命令提示符线程
    createCommandlinePosixThread(&commandline, &ppc);
    // 打开音频线程
    property.sound.soundPause = false;
    // 启动混成器
    frameCompositor(&property, &fileMetaData, METADATA_HEADER_SIZE + 1);

    // 释放占用的资源
    freeSystemResources(&property);
    
    // 等待线程退出
    puts("\nWaitting for playback thread...");
    pthread_join(playbackThread, NULL);
    puts("Waitting for commandline thread...");
    pthread_cancel(commandline);

    return 0;
}
