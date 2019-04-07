#ifndef _SUBCMDS_H
#define _SUBCMDS_H 1

#include "main.h"
#include "threads.h"

// 注册命令
#define REGCMD(name)    void __subcmd_##name (threadsPPC_t *ppc, char *args[])

// 注册命令指针 （用于 subcmdPionter_t 结构体）
#define DEFCMDP(name)   void *(* name) (threadsPPC_t *ppc, char *args[])

// 通过命令名解析到指针名
#define RESOLVECMD(structName, cmd, ppc, args) structName->cmd(ppc, args)

// 反向解析（通过命令名解析到实现函数名）
#define REVERSE_RESCMD(name) (void *)__subcmd_##name


/* 最关键的结构体，保存了每个命令实现的函数指针 */
typedef struct _Subcmds_Function_Struct {
    DEFCMDP(metadata);
    DEFCMDP(help);
    DEFCMDP(audioctl);
    DEFCMDP(framectl);
    DEFCMDP(getprop);
    DEFCMDP(disable);
    DEFCMDP(enable);
} subcmdPionter_t;

void commandLineProcessService(threadsPPC_t *ppc, subcmdPionter_t *cmdpionter);

/**
 * subcmdPionter_t 结构体中每一个成员都默认指向该函数（除了內建命令 version）。
 * 直到它们的值被 __init_subcmd_pionters 函数更改。
 * 此举是为了防止用户调用不存在的命令访问了空指针。
*/
void *__subcmd_standard_null_pionter (threadsPPC_t *a, char *b[]);

/**
 * 初始化成员指针。用户定义的命令全部在这里注册。
*/
void __init_subcmd_pionters(subcmdPionter_t *table);


/**
 * 用户定义的命令实现。无论是否需要，强制要求参数格式为
 * (threadsPPC_t *, char const *args)。
 * 第一个参数传入 threads.h::threadsPPC_t 结构体指针，
 * 它是 melter 提供的标准线程间通信 (PPC) 机制；第二个参数
 * 指向一个字符串指针，它是提供给函数的参数字符串，包含经过格式化预处理
 * 的命令参数。比如，用户输入
 * `setprop --force-rw compositor.skippingFrameLimit 0.996'，
 * 则传给 setprop 函数的参数字符串是
 * `arg[0]--force-rw arg[1]compositor.skippingFrameLimit arg[2]0.996'。
 * 去除了多余的空格并以空字符作为参数块分割标记。
*/
REGCMD(metadata);
REGCMD(help);
REGCMD(audioctl);
REGCMD(framectl);
REGCMD(getprop);
REGCMD(disable);
REGCMD(enable);

#endif