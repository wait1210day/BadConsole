#include "threads.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include <readline/readline.h>
#include <readline/history.h>

#include "subcmds.h"

#define CMPI(str)               if (strcmp(buffer, str) == 0)
#define CMPIELSE(str)           else if (strcmp(buffer, str) == 0)
#define CMPIAND(str1, str2)     if (strcmp(buffer, str1) == 0 && strcmp(buffer, str2) == 0)
#define CMPIOR(str1, str2)      if (strcmp(buffer, str1) == 0 || strcmp(buffer, str2) == 0)
#define ECMPIAND(str1, str2)    else if (strcmp(buffer, str1) == 0 && strcmp(buffer, str2) == 0)
#define ECMPIOR(str1, str2)     else if (strcmp(buffer, str1) == 0 || strcmp(buffer, str2) == 0)

#define ARGBUFSIZ   20
#define ARGPIONTERBUF 10


int createCommandlinePosixThread(pthread_t *id, threadsPPC_t *ppc) {
    ppc->prop->prompt.cmdPause = true;
    ppc->prop->prompt.cmdTriggerTime = (int) ppc->prop->timeSpeed * 10;

    if (pthread_create(id, NULL, (void *)commandPrompt, ppc) < 0) {
        printf("Melter: Cannot create commandline thread\n");
        return -1;
    }

    return 0;
}

void commandPrompt(threadsPPC_t *ppc) {
    char privateBuffer[1];
    subcmdPionter_t cmdpionter;

    // 初始化命令函数指针结构体
    __init_subcmd_pionters(&cmdpionter);

    while (1) {
        while (ppc->prop->prompt.cmdPause)
            usleep (ppc->prop->prompt.cmdTriggerTime);
        // 读取一个字节内容
        read(STDIN_FILENO, privateBuffer, 1);

        // 为换行符，触发操作
        if (privateBuffer[0] == '\n') {
            // 停止混成器和音频服务
            ppc->prop->sound.soundPause = true;
            ppc->prop->compositorPause = true;

            // 调用处理函数
            commandLineProcessService(ppc, &cmdpionter);

            // 继续混成工作
            ppc->prop->sound.soundPause = false;
            ppc->prop->compositorPause = false;
        }
        privateBuffer[0] = '\0';
    }
}

/**
 * 初始化参数指针数组，私有函数仅限本文件内部使用
*/
void __subcmd_utils_init_arg_pionter_buf (char *buf[]) {
    for (int i = 0; i != ARGPIONTERBUF; i++)
        buf[i] = NULL;
}

/**
 * 释放参数指针数组占用的内存空间。私有函数惊险本文件内部使用
*/
void __subcmd_utils_free_arg_pionter_buf (char *buf[]) {
    for (int i = 0; buf[i] != NULL; i++) {
        free(buf[i]);
        // 防止野指针问题
        buf[i] = NULL;
    }
}

void commandLineProcessService(threadsPPC_t *ppc, subcmdPionter_t *cmdpionter) {
    char *input = NULL;
    char buffer[20];
    // 每个命令的参数
    char *argbuf = malloc(ARGBUFSIZ);
    // 保存指针的数组
    char *args[ARGPIONTERBUF];

    int argsAddr = 0;
    int i;

    // 初始化参数指针
    __subcmd_utils_init_arg_pionter_buf (args);
    memset(argbuf, '\0', ARGBUFSIZ);

    // 进入主循环
    while (1) {
        if (input != NULL) free(input);

        input = readline("\033[33;1mCompositor\033[0m > ");
        memset(buffer, '\0', 20);

        // 拒绝处理空字符和换行符，要求用户重新输入
        if (input[0] == '\0' || input[0] == ' ' || input[0] == '\n') continue;

        // 拆分解析输入
        for (i = 0; input[i] != ' ' && input[i] != '\n' && input[i] != '\0'; i++) {
            // 超出数组下标上限，强制截断。
            if (i >= 20) continue;

            buffer[i] = input[i];
        }
        i++;

        for (int j = 0; j != ARGBUFSIZ; i++) {
            if (input[i] == ' ' || input[i] == '\0') {
                // printf("\033[31m Accept: %s\n\033[0m", argbuf);
                // 提交字符串到指针数组
                args[argsAddr] = argbuf;
                argsAddr++;
                // 重新分配空间
                argbuf = malloc(ARGBUFSIZ);
                memset(argbuf, '\0', ARGBUFSIZ);
                j = 0;

                if (input[i] == '\0') break;

                continue;

            } else {
                // printf("Copy input[%d] => argbuf[%d] (%d)\n", i, j, input[i]);
                argbuf[j] = input[i];
                j++;
            }
        }

        CMPIOR("quit", "q") {
            break;

        } ECMPIOR("break", "b") {
            ppc->prop->sound.soundBreak = true;
            ppc->prop->compositorBreak = true;

        } ECMPIOR("version", "ver") {
            puts("Melter: Example program of Melter File Player");
            puts("Version: v1.0-developer (Will be: v1.0-released)");
            puts("Shell Version: Melter Compositor Console (v1.1-developer)");

        } ECMPIOR("metadata", "m") {
            RESOLVECMD(cmdpionter, metadata, ppc, args);
        } ECMPIOR("help", "h") {
            RESOLVECMD(cmdpionter, help, ppc, args);
        } ECMPIOR("audioctl", "a") {
            RESOLVECMD(cmdpionter, audioctl, ppc, args);
        } ECMPIOR("framectl", "f") {
            RESOLVECMD(cmdpionter, framectl, ppc, args);
        } ECMPIOR("getprop", "g") {
            RESOLVECMD(cmdpionter, getprop, ppc, args);
        } ECMPIOR("disable", "d") {
            RESOLVECMD(cmdpionter, disable, ppc, args);
        } ECMPIOR("enable", "e") {
            RESOLVECMD(cmdpionter, enable, ppc, args);
        } else {
            puts("\033[31m[!] Command Not Found. Use `help\' for more informations\033[0m");
        }

        // 释放指针
        __subcmd_utils_free_arg_pionter_buf (args);
        argsAddr = 0;
    }

    __subcmd_utils_free_arg_pionter_buf (args);
    free(input);
}