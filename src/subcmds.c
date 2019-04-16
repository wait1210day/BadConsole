#include "subcmds.h"
#include <stdio.h>

void *__subcmd_standard_null_pionter (threadsPPC_t *a, char *b[]) {
    printf("MelterConsole: DeveloperDebug & Error: Cannot resolve command address\n");
    printf("MelterConsole: DeveloperDebug: in function `__subcmd_standard_null_pionter\': Null Pointer Dereference\n");
    printf("MelterConsole: DeveloperDebug: ARGS = ");
    for (int i = 0; b[i] != NULL; i++) {
        printf("[%d]%s ", i, b[i]);
    }
    putchar('\n');

    return NULL;
}

void __init_subcmd_pionters(subcmdPionter_t *table) {
    table->audioctl = __subcmd_standard_null_pionter;
    table->disable = __subcmd_standard_null_pionter;
    table->enable = __subcmd_standard_null_pionter;
    table->framectl = __subcmd_standard_null_pionter;
    table->getprop = __subcmd_standard_null_pionter;
    table->help = __subcmd_standard_null_pionter;
    table->metadata = __subcmd_standard_null_pionter;

    // 在这里写入代码覆盖默认设置
    table->help = REVERSE_RESCMD(help);
}


// 所有子命令的实现函数：

/**
 * help 命令输出 MulterConsole 的所有內建命令帮助信息
 * 
*/
REGCMD (help) {
    puts ("\033[34mMelterConsole Built-in Commands\033[0m");
    puts ("\033[32mUsage:\033[0m [<version>|<help>|<...>] arg1 arg2 arg3 \033[31m...\033[0m");
    puts ("\nBuiltin Commands: ");
    puts (" [B]reak        立刻终止混成和音频服务进程");
    puts (" [Q]uit         退出 MelterConsole");
    puts ("\0");
    puts ("\033[34mWhat is `Null Pointer Dereference\'? :\033[0m");
    puts ("  MelterConsole 采用结构体保存每个命令实现函数的指针，“空指针解除引用” 是一种运行时安全保护机制。");
    puts (" 引用的目标函数可能寻址失败或出现其它问题，导致程序访问到 0x0 内存空间，除非运行于 x86 实模式，");
    puts (" 否则会引起段错误。函数 subcmds::__subcmd_standard_null_pionter 用于解决此问题，一旦寻");
    puts (" 址错误，此函数会被调用并输出 `Null Pointer Dereference\' 信息。此函数输出的提示级别较为");
    puts (" 特殊，为 `DeveloperDebug & Error\'，类似于 Java 或 Python 的 `Call Back Trace\'");
    puts (" 此问题很难解决，最可能的原因是您使用 REGCMD() 注册了命令但是没有使用 REGCMD() { ... } 提供实现");
    puts ("\0");
    puts ("\033[34mHow to add a new command to MelterConsole:\033[0m");
    puts ("  在文件 subcmds.h 中，添加行 REGCMD(\033[33mcmd_name\033[0m)");
    puts ("  在结构体 subcmds.h::_Subcmds_Function_Struct 中添加成员 DEFCMDP(\033[33mcmd_name\033[0m)");
    puts ("  在函数 subcmds.c::__init_subcmd_pionters 适当位置添加行 table->\033[33mcmd_name\033[0m = __subcmd_standard_null_pionter");
    puts ("  在函数 subcmds.c::__init_subcmd_pionters 适当位置添加行 table->\033[33mcmd_name\033[0m = REVERSE_RESCMD(\033[33mcmd_name\033[0m)");
    puts ("  在 subcmd.c 文件末尾添加函数实现 REGCMD(\033[33mcmd_name\033[0m) { ... }");
}