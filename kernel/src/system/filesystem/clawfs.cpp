#include "clawfs.h"
#include "system/drivers/video/driver.h"
#include "libs/libc/libc.h"
#include "system/drivers/ata/driver.h"
#include "applications/shell/commands.h"

void clawfs_format()
{
    // 1. Formating header (Sektor 100)
    uint8_t sector_buffer[512] = {0};
    CLAWFSHeader* header = (CLAWFSHeader*)sector_buffer;

    header->signature[0] = 'C';
    header->signature[1] = 'L';
    header->signature[2] = 'A';
    header->signature[3] = 'W';
    header->signature[4] = 'F';
    header->signature[5] = 'S';
    header->version = CLAWFS_VERSION;
    header->entryCount = 4;

    disk_write_sector(CLAWFS_START_LBA, sector_buffer);

    uint8_t entries_buffer[512] = {0};
    CLAWFSEntry* entries = (CLAWFSEntry*)entries_buffer;

    strcpy(entries[0].name, "bin");
    entries[0].type = CLAWFS_DIRECTORY;

    strcpy(entries[1].name, "dev");
    entries[1].type = CLAWFS_DIRECTORY;

    strcpy(entries[2].name, "etc");
    entries[2].type = CLAWFS_DIRECTORY;

    strcpy(entries[3].name, "home");
    entries[3].type = CLAWFS_DIRECTORY;

    disk_write_sector(CLAWFS_START_LBA + 1, entries_buffer);
}

void clawfs_create_file(const char* name)
{
    uint8_t header_buffer[512];
    disk_read_sector(CLAWFS_START_LBA, header_buffer);
    CLAWFSHeader* header = (CLAWFSHeader*)header_buffer;

    uint8_t search_buffer[512];
    uint32_t current_loaded_sector = 0;
    CLAWFSEntry* search_entries = nullptr;

    for (uint32_t i = 0; i < header->entryCount; i++)
    {
        uint32_t required_sector_offset = 1 + (i / 16);
        uint32_t index_in_sector = i % 16;

        if (current_loaded_sector != required_sector_offset) 
        {
            disk_read_sector(CLAWFS_START_LBA + required_sector_offset, search_buffer);
            current_loaded_sector = required_sector_offset;
            search_entries = (CLAWFSEntry*)search_buffer;
        }

        if (strcmp(search_entries[index_in_sector].name, name) == 0) 
        {
            print_error("File '");
            print(name);
            print("' aleredy exists!\n");
            return;
        }
    }

    if (header->entryCount >= 128) 
    {
        print_error("No space anvible\n");
        return;
    }

    uint32_t target_index = header->entryCount;
    uint32_t sector_offset = 1 + (target_index / 16);
    uint32_t index_in_sector = target_index % 16;

    uint8_t entries_buffer[512];
    disk_read_sector(CLAWFS_START_LBA + sector_offset, entries_buffer);
    CLAWFSEntry* entries = (CLAWFSEntry*)entries_buffer;

    strcpy(entries[index_in_sector].name, name);
    entries[index_in_sector].type = CLAWFS_FILE;

    disk_write_sector(CLAWFS_START_LBA + sector_offset, entries_buffer);

    header->entryCount++;
    disk_write_sector(CLAWFS_START_LBA, header_buffer);

    print_info("File created: ");
    print(name);
    print("\n");
}

void clawfs_dir()
{
    uint8_t header_buffer[512];
    disk_read_sector(CLAWFS_START_LBA, header_buffer);
    CLAWFSHeader* header = (CLAWFSHeader*)header_buffer;

    uint8_t entries_buffer[512];
    uint32_t current_loaded_sector = 0;
    CLAWFSEntry* entries = nullptr;

    for(uint32_t i = 0; i < header->entryCount; i++)
    {
        // Dynamic calc where is data
        uint32_t required_sector_offset = 1 + (i / 16);
        uint32_t index_in_sector = i % 16;

        // Read sector only if it is not in RAM
        if (current_loaded_sector != required_sector_offset) 
        {
            disk_read_sector(CLAWFS_START_LBA + required_sector_offset, entries_buffer);
            current_loaded_sector = required_sector_offset;
            entries = (CLAWFSEntry*)entries_buffer;
        }

        if(entries[index_in_sector].type == CLAWFS_DIRECTORY)
        {
            print("<DIR> ");
        }
        else
        {
            print("      ");
        }

        print(entries[index_in_sector].name);
        print("\n");
    }
}

bool clawfs_exists()
{
    uint8_t buffer[512];
    disk_read_sector(CLAWFS_START_LBA, buffer);
    CLAWFSHeader* header = (CLAWFSHeader*)buffer;

    bool isValid = (
        header->signature[0] == 'C' &&
        header->signature[1] == 'L' &&
        header->signature[2] == 'A' &&
        header->signature[3] == 'W' &&
        header->signature[4] == 'F' &&
        header->signature[5] == 'S' &&
        header->version == CLAWFS_VERSION
    );

    if (isValid) 
    {
        print("Partition Mounted\n");
    }
    
    return isValid;
}