#ifndef _TOOLKIT_H
#define _TOOLKIT_H 1

#include "main.h"
#ifdef __gnu_linux__
#include <pulse/simple.h>
#endif

/**
 * 字符串相似度处理函数，skip 参数表示先跳过 str1 的 skip 个字符再进行比较。
 * 返回比例制的浮点 double 值。
*/
double compareStr (char const *str1, char const *str2, unsigned long size, unsigned short skip);

/**
 * 初始化 PosixThreadIPC 结构体。
 * 第二个参数是指向 argv 的指针。
*/
int initDataStruct (PosixThreadIPC_t *property, char **argv, int argc);

/**
 * 释放内存并关闭一些文件描述符。
 * 必须与 initDataStruct 函数配对使用。
*/
int freeSystemResources (PosixThreadIPC_t *property);

/**
 * melted 文件头部解析器
*/
int parseMeltedFileHeader (PosixThreadIPC_t *property, Metadata_t *metadata);

/**
 * 调试输出
*/
void printMetadata (Metadata_t *m);

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
    int *error
);
int __default_pa_simple_write (pa_simple *s, const void *data, size_t bytes, int *error);
int __default_pa_simple_drain (pa_simple *s, int *error);
void __default_pa_simple_free (pa_simple *s);
/**
 * 加载 PulseAudio 库函数
*/
int loadPulseaudioDynamicLib (threadsPPC_t *ppc);

/**
 * 释放 PulseAudio 函数库
*/
void freePulseaudioDynamicLib (threadsPPC_t *ppc);

#endif

#endif // _TOOLKIT_H