#ifndef _TOOLKIT_H
#define _TOOLKIT_H 1

#include "main.h"

/**
 * 字符串相似度处理函数，skip 参数表示先跳过 str1 的 skip 个字符再进行比较。
 * 返回比例制的浮点 double 值。
*/
double compareStr(char const *str1, char const *str2, unsigned long size, unsigned short skip);

/**
 * 初始化 PosixThreadIPC 结构体。
 * 第二个参数是指向 argv 的指针。
*/
int initDataStruct(PosixThreadIPC_t *property, char const **argv, int argc);

/**
 * 释放内存并关闭一些文件描述符。
 * 必须与 initDataStruct 函数配对使用。
*/
int freeSystemResources(PosixThreadIPC_t *property);

/**
 * melted 文件头部解析器
*/
int parseMeltedFileHeader(PosixThreadIPC_t *property, Metadata_t *metadata);

/**
 * 调试输出
*/
void printMetadata(Metadata_t *m);

#endif // _TOOLKIT_H