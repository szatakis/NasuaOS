#pragma once
#include <stdint.h>

#define ZTRFS_VERSION 1
#define ZTRFS_START_LBA 100 

#define ZTRFS_FILE 0
#define ZTRFS_DIRECTORY 1

struct ZTRFSHeader {
    char signature[5];
    uint8_t version;
    uint16_t entryCount;
} __attribute__((packed));

struct ZTRFSEntry {
    char name[28];
    uint32_t type;
} __attribute__((packed));

void ztrfs_format();
void ztrfs_dir();
bool ztrfs_exists();
void ztrfs_create_file(const char* name); // Nowa funkcja