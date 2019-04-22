#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>


#include "metadata.h"
#include "main.h"
#include "compositor.h"
#include "toolkit.h"
#include "threads.h"
#include "subcmds.h"

int main(int argc, char **argv) {
    int cancel_val = 1;

    Metadata_t fileMetaData;
    PosixThreadIPC_t property;
    threadsPPC_t ppc;
    pthread_t playbackThread;
    pthread_t commandline;
    pthread_t composition;
    ppc.md = &fileMetaData;
    ppc.prop = &property;

    // 初始化结构体并打开相应文件
    if (initDataStruct(&property, argv, argc) < 0)
        goto _end;

    // 加载文件和库
    if (parseMeltedFileHeader (&property, &fileMetaData) < 0)
        goto _end;
    printMetadata (&fileMetaData);

#ifdef __gnu_linux__
    if (loadWAVDataFromPath (&ppc) < 0)
        goto _end;
    loadPulseaudioDynamicLib (&ppc);
#endif

    // 创建线程
    createCommandlinePosixThread (&commandline, &ppc);
#ifdef __gnu_linux__
    createPlaybackPosixThread    (&playbackThread, &ppc, 2, 44100);
#endif
    createCompositionPosixThread (&composition, &ppc, METADATA_HEADER_SIZE + 1);

    // 等待确认输入
    printf ("\033[31;1m[!] Press Enter key to start compositor...\033[0m\n");
    getc (stdin);

    // 通知线程开始运行
    property.prompt.cmdPause = false;
    property.sound.soundPause = false;
    property.compositorPause = false;
    
    // 等待线程退出
    pthread_join (composition, NULL);
#ifdef __gnu_linux__
    pthread_join (playbackThread, NULL);
#endif
    pthread_cancel (commandline);

    // 执行到这一步说明不是 goto 过来的，说明程序没有发生错误退出
    cancel_val = 0;

_end:
    // 释放占用的资源
    freeSystemResources (&property);

#ifdef __gnu_linux__
    freePulseaudioDynamicLib (&ppc);
#endif

    return cancel_val;
}
