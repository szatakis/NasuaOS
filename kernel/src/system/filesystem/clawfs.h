#pragma once
#include <stdint.h>

#define CLAWFS_VERSION 1
#define CLAWFS_START_LBA 100 

#define CLAWFS_FILE 0
#define CLAWFS_DIRECTORY 1

struct CLAWFSHeader 
{
    char signature[6];
    uint8_t version;
    uint16_t entryCount;
} __attribute__((packed));

struct CLAWFSEntry 
{
    char name[28];
    uint32_t type;
} __attribute__((packed));

void clawfs_format();
void clawfs_dir();
bool clawfs_exists();
void clawfs_create_file(const char* name);