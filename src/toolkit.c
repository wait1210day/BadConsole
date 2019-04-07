#include "metadata.h"
#include "toolkit.h"
#include "main.h"

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

double compareStr(char const *str1, char const *str2, unsigned long size, unsigned short skip) {
    unsigned long same = 0;

    for (unsigned long i = 0; i != size; i++) {
        if (str1[i + skip] == str2[i])
            same++;
    }
    return (double) same / (double) size;
}

int initDataStruct(PosixThreadIPC_t *property, char const **argv, int argc) {
    property->compositorPause = false;
    property->toBeContinue = true;
    property->skipedFrames = 0;
    property->sameRate = 0;
    
    if (argc < 2) {
        fprintf(stderr, "Melter: Cannot resolve arguments\n");
        return -1;
    }

    property->args.meltedFile = argv[1];
    property->args.timeSpeed = atof(argv[2]) * 1000;
    property->args.skippingFrameRate = atof(argv[3]);
    property->compositorTriggerTime = (int)property->args.timeSpeed * 10;

    property->meltFileDescriptor = open(property->args.meltedFile, O_RDWR);
    if (property->meltFileDescriptor < 0) {
        perror("Melter: Cannot open");
        return -1;
    }
    return 0;
}

int freeSystemResources(PosixThreadIPC_t *property) {
    if (property->buffer != NULL) {
        free(property->buffer);
        free(property->bigBuffer);
        free(property->frameBuffer);
        free(property->frameBuffer_Back);
    }
    close(property->meltFileDescriptor);

    property->buffer = NULL;
    property->bigBuffer = NULL;
    property->frameBuffer = NULL;
    property->frameBuffer_Back = NULL;
    property->meltFileDescriptor = -1;

    return 0;
}

void __cutSection(char const *src, char *dest, int n) {
    memset(dest, '\0', n);

    int i;
    for (i = 0; src[i] != ' '; i++)
        ;
    i++;

    for (int a = 0; src[i] != '\0'; i++, a++)
        dest[a] = src[i];
}

int parseMeltedFileHeader(PosixThreadIPC_t *property, Metadata_t *metadata) {
    // 根据文件格式规定，前 metadata.h::METADATA_HEADER_SIZE(395) 字节为元数据信息，不足部分使用空字符补齐
    // 每一行具有 METADATA_SECTION_SIZE(41) 的标准长度 （包含换行符），不足部分用空字符补齐
    char lineBuffer[METADATA_SECTION_SIZE];
    char numberBuffer[8];
    int res = 1;

    // 读取前 27 字节，为文件类型标识符
    memset(metadata->formatDescriptor, '\0', METADATA_SECTION_FD_SIZE);
    res *= read(property->meltFileDescriptor, metadata->formatDescriptor, METADATA_SECTION_FD_SIZE);
    if (res < 0) {
        perror("Melter: parseMeltedFileHeader: Cannot read");
        return -1;
    }
    if (!strncmp(metadata->formatDescriptor, "@FormatDescriptor MELTDATA\n", METADATA_SECTION_FD_SIZE)) {
        printf("Melter: File format is right\n");
    } else {
        fprintf(stderr, "Melter: %s: Header: Looks like a broken file\n", property->args.meltedFile);
        printf("Broken header: %s\n", metadata->formatDescriptor);
        return -1;
    }

    // 初始化变量
    memset(metadata->author, '\0', METADATA_SECTION_SIZE);
    memset(metadata->highLight, '\0', 4);
    memset(metadata->RGBOption, '\0', 5);
    memset(metadata->timeStamp, '\0', METADATA_SECTION_SIZE);
    memset(metadata->version, '\0', METADATA_SECTION_SIZE);
    metadata->displayPerformace = true;
    metadata->frameHeight = 0;
    metadata->frameWidth = 0;
    metadata->frameSize = 0;
    
    // 开始正式读取
    res *= read(property->meltFileDescriptor, metadata->version, METADATA_SECTION_SIZE);
    res *= read(property->meltFileDescriptor, metadata->timeStamp, METADATA_SECTION_SIZE);
    res *= read(property->meltFileDescriptor, lineBuffer, METADATA_SECTION_SIZE);
    if (res < 0) {
        perror("Melter: parseMeltedFileHeader: Cannot read");
        return -1;
    }
    if (compareStr(lineBuffer, "FrameWidth", 10, 0) < 1) {
        fprintf(stderr, "Melter: %s: Looks like a broken file\n", property->args.meltedFile);
        return -1;
    } else {
        __cutSection(lineBuffer, numberBuffer, 8);
        metadata->frameWidth = atoi(numberBuffer);
    }

    read(property->meltFileDescriptor, lineBuffer, METADATA_SECTION_SIZE);
    if (compareStr(lineBuffer, "FrameHeigth", 11, 0) < 1) {
        fprintf(stderr, "Melter: %s: Looks like a broken file\n", property->args.meltedFile);
        return -1;
    } else {
        __cutSection(lineBuffer, numberBuffer, 8);
        metadata->frameHeight = atoi(numberBuffer);
    }
    metadata->frameSize = metadata->frameHeight * (metadata->frameWidth + 2);

    read(property->meltFileDescriptor, lineBuffer, METADATA_SECTION_SIZE);
    if (compareStr(lineBuffer, "RGBOption", 9, 0) < 1) {
        fprintf(stderr, "Melter: %s: Looks like a broken file\n", property->args.meltedFile);
        return -1;
    } else {
        __cutSection(lineBuffer, numberBuffer, 8);
        if (!strcmp(numberBuffer, "Red")) {
            strcpy(metadata->RGBOption, "\033[31m");
        } else if (!strcmp(numberBuffer, "Green")) {
            strcpy(metadata->RGBOption, "\033[32m");
        } else if (!strcmp(numberBuffer, "Yellow")) {
            strcpy(metadata->RGBOption, "\033[33m");
        } else if (!strcmp(numberBuffer, "Blue")) {
            strcpy(metadata->RGBOption, "\033[34m");
        } else if (!strcmp(numberBuffer, "Purple")) {
            strcpy(metadata->RGBOption, "\033[35m");
        } else if (!strcmp(numberBuffer, "White")) {
            strcpy(metadata->RGBOption, "\033[37m");
        } else {
            strcpy(metadata->RGBOption, "\0\0\0\0\0");
        }
    }

    return 0;
}

void printMetadata(Metadata_t *m) {
    printf("Metadata: %s\n", m->version);
    printf("Metadata: %s\n", m->timeStamp);
    printf("Metadata: FrameHeight %d\n", m->frameHeight);
    printf("Metadata: FrameWidth %d\n", m->frameWidth);
    printf("Metadata: FrameSize %ld\n", m->frameSize);
    printf("Metadata: RGBOption %sRGBTest\033[0m\n", m->RGBOption);
}