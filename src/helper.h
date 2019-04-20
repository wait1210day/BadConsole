#ifndef __HELPER_INCLUDED__
#define __HELPER_INCLUDED__ 1

#include "main.h"

/**
 * 本文件给出作者信息以及处理 `--help' 参数的函数
*/

#define VBC_author          "Jingheng Luo (masshiro.io@qq.com)"
#define VBC_version         "1.0.2-developer (master)"
#define VBC_license         "GNU General Public License (Version 3, 29 June 2007)"
#define VBC_copyrightTime   "2019"
#define VBC_platform        "Linux"
#define VBC_builder         "5.0.7-arch1-1-ARCH, GCC-8.2.1, Binutils-2.31.1"
#define VBC_language        "English (United States)"
#ifdef CCB_enable_pulseaudio
#define VBC_PA              "GNU/Linux PulseAudio"
#else
#define VBC_PA              "Non-GNU/Linux Unknow"
#endif // CCB_enable_pulseaudio


void helper_displayHelp (char const *cmda);
void helper_displayVersion (void);
void helper_displayDetails (void);

#endif