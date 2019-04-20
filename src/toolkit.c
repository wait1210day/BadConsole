#include "metadata.h"
#include "toolkit.h"
#include "main.h"
#include "helper.h"

#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <dlfcn.h>

static char const getopt_shortopts[] = "hvdm:t:f:w:l:";
static struct option getopt_longopts[] = {
    { "help", no_argument, NULL, 'h' },
    { "version", no_argument, NULL, 'v' },
    { "details", no_argument, NULL, 'd' },
    { "melt-file", required_argument, NULL, 'm' },
    { "time-speed", required_argument, NULL, 't' },
    { "frame-skipping-rate", required_argument, NULL, 'f' },
    { "audio-wav", required_argument, NULL, 'w' },
    { "pulseaudio-dynamic-lib", required_argument, NULL, 'l' },
    { NULL, no_argument, NULL, 0 }
};

double compareStr(char const *str1, char const *str2, unsigned long size, unsigned short skip) {
    unsigned long same = 0;

    for (unsigned long i = 0; i != size; i++) {
        if (str1[i + skip] == str2[i])
            same++;
    }
    return (double) same / (double) size;
}

int initDataStruct(PosixThreadIPC_t *property, char **argv, int argc) {
    property->buffer            = NULL;
    property->bigBuffer         = NULL;
    property->frameBuffer       = NULL;
    property->frameBuffer_Back  = NULL;
    property->args.meltedFile   = NULL;
    property->args.wavFile      = NULL;
    property->args.pulseLib     = NULL;
    property->sound.pulseaudio  = NULL;
    property->sound.soundBreak  = false;
    property->sound.soundPause  = true;
    property->compositorBreak   = false;
    property->compositorPause   = true;
    property->toBeContinue      = true;
    property->skipedFrames      = 0;
    property->sameRate          = 0;
    
    if (argc < 2) {
        fprintf(stderr, "Melter: Cannot resolve arguments\n");
        return -1;
    }

    int c;
    while ((c = getopt_long (argc, (char *const *)argv, getopt_shortopts, getopt_longopts, NULL)) != -1) {
        switch (c) {
            case 'h':
                helper_displayHelp (argv[0]);
                freeSystemResources (property);

                exit (EXIT_SUCCESS);
                break;
            case 'v':
                helper_displayVersion ();
                freeSystemResources (property);

                exit (EXIT_SUCCESS);
                break;
            case 'd':
                helper_displayDetails ();
                freeSystemResources (property);
                
                exit (EXIT_SUCCESS);
                break;
            case 'm':
                property->args.meltedFile = malloc (strlen(optarg));
                strcpy (property->args.meltedFile, optarg);
                break;
            case 't':
                property->args.timeSpeed = atof(optarg) * 1000;
                break;
            case 'f':
                property->args.skippingFrameRate = atof(optarg);
                break;
            case 'w':
                property->args.wavFile = malloc (strlen(optarg));
                strcpy (property->args.wavFile, optarg);
                break;
            case 'l':
                property->args.pulseLib = malloc (strlen (optarg));
                strcpy (property->args.pulseLib, optarg);
                break;
        }
    }

    /*
    property->args.meltedFile = argv[1];
    property->args.timeSpeed = atof(argv[2]) * 1000;
    property->args.skippingFrameRate = atof(argv[3]);
    */

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

    if (property->meltFileDescriptor > 0)
        close(property->meltFileDescriptor);

    property->buffer = NULL;
    property->bigBuffer = NULL;
    property->frameBuffer = NULL;
    property->frameBuffer_Back = NULL;
    property->meltFileDescriptor = -1;

    if (property->args.meltedFile != NULL) free (property->args.meltedFile);
    if (property->args.wavFile != NULL) free (property->args.wavFile);
    if (property->args.pulseLib != NULL) free (property->args.pulseLib);

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

#ifdef __gnu_linux__

pa_simple *__default_pa_simple_new (
    const char *server,
    const char *name,
    pa_stream_direction_t dir,
    const char *dev,
    const char *stream_name,
    const pa_sample_spec *ss,
    const pa_channel_map *map,
    const pa_buffer_attr *attr,
    int *error) {


    return NULL;
}
int __default_pa_simple_write(pa_simple *s, const void *data, size_t bytes, int *error) {
    return 1;
}
int __default_pa_simple_drain (pa_simple *s, int *error) {
    return -1;
}
void __default_pa_simple_free (pa_simple *s) {
    return ;
}

/**
 * 加载 PulseAudio 库函数
*/
int loadPulseaudioDynamicLib (threadsPPC_t *ppc) {
    char *path = ppc->prop->args.pulseLib;

    ppc->prop->sound.pulseaudio = dlopen (path, RTLD_NOW);
    if (ppc->prop->sound.pulseaudio == NULL) {
        ppc->prop->sound.pa_simple_new = __default_pa_simple_new;
        return -1;
    }

    ppc->prop->sound.pa_simple_new = dlsym (ppc->prop->sound.pulseaudio, "pa_simple_new");
    ppc->prop->sound.pa_simple_write = dlsym (ppc->prop->sound.pulseaudio, "pa_simple_write");
    ppc->prop->sound.pa_simple_drain = dlsym (ppc->prop->sound.pulseaudio, "pa_simple_drain");
    ppc->prop->sound.pa_simple_free = dlsym (ppc->prop->sound.pulseaudio, "pa_simple_free");

    if (ppc->prop->sound.pa_simple_new == NULL) ppc->prop->sound.pa_simple_new = __default_pa_simple_new;
    if (ppc->prop->sound.pa_simple_write == NULL) ppc->prop->sound.pa_simple_write = __default_pa_simple_write;
    if (ppc->prop->sound.pa_simple_drain == NULL) ppc->prop->sound.pa_simple_drain = __default_pa_simple_drain;
    if (ppc->prop->sound.pa_simple_free == NULL) ppc->prop->sound.pa_simple_free = __default_pa_simple_free;

    return 0;
}

/**
 * 释放 PulseAudio 函数库
*/
void freePulseaudioDynamicLib (threadsPPC_t *ppc) {
    if (ppc->prop->sound.pulseaudio != NULL)
        dlclose (ppc->prop->sound.pulseaudio);
}

#endif