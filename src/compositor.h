#ifndef _COMPOSITOR_H
#define _COMPOSITOR_H 1

#include "main.h"

/**
 * 帧混成与输出函数。接受一个参数，指向 PosixThreadIPC_t
 * 结构体的指针
*/
void frameCompositor(threadsPPC_t *ppc);

#endif // _COMPSITOR_H