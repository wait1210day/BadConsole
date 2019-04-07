#ifndef _METADATA_H
#define _METADATA_H 1

#define METADATA_HEADER_SIZE        395
#define METADATA_SECTION_FD_SIZE    27
#define METADATA_SECTION_SIZE       41

#define MD_RGBOPTION_NORMAL     0


#include <stdbool.h>
// 保存 melted 文件的元数据
typedef struct _MetaData_Struct {
    char formatDescriptor[METADATA_SECTION_FD_SIZE];
    char version[METADATA_SECTION_SIZE];
    char timeStamp[METADATA_SECTION_SIZE];
    unsigned int frameWidth;
    unsigned int frameHeight;
    unsigned long int frameSize;
    char RGBOption[5];
    char highLight[4];
    bool displayPerformace;
    char author[METADATA_SECTION_SIZE];
} Metadata_t;

#endif // _METADATA_H