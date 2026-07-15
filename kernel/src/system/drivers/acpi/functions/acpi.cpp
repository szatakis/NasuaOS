#include "../driver.h"
#include <stdint.h>
#include <stddef.h>
#include <limine.h>

//fuck to person who invented this 🖕

#include "../../video/driver.h"
#include "libs/asm/asm.h"
#include "applications/shell/commands.h"

extern "C" volatile struct limine_rsdp_request rsdp_request;
extern "C" volatile struct limine_hhdm_request hhdm_request;

// ===================== //
// GLOBAL ACPI VARIABLES //
// ===================== //

static uint32_t SMI_CMD = 0;
static uint8_t ACPI_ENABLE = 0;
static uint32_t PM1a_CNT = 0;
static uint32_t PM1b_CNT = 0;
static uint32_t PM1a_EVT = 0; 
static uint16_t SLP_TYPa = 0;
static uint16_t SLP_TYPb = 0;
static uint16_t SLP_EN = 0;
static uint16_t SCI_EN = 0;

// ===================== //
// ACPI STRUCTURES       //
// ===================== //

struct __attribute__((packed)) ACPISDTHeader {
    char Signature[4];
    uint32_t Length;
    uint8_t Revision;
    uint8_t Checksum;
    char OEMID[6];
    char OEMTableID[8];
    uint32_t OEMRevision;
    uint32_t CreatorID;
    uint32_t CreatorRevision;
};

struct __attribute__((packed)) RSDP {
    char signature[8];
    uint8_t checksum;
    char oemid[6];
    uint8_t revision;
    uint32_t rsdt_address;
    uint32_t length;
    uint64_t xsdt_address;
    uint8_t extended_checksum;
    uint8_t reserved[3];
};

struct __attribute__((packed)) GAS {
    uint8_t AddressSpace;
    uint8_t BitWidth;
    uint8_t BitOffset;
    uint8_t AccessSize;
    uint64_t Address;
};

struct __attribute__((packed)) FADT {
    ACPISDTHeader header;
    uint32_t FirmwareCtrl;
    uint32_t Dsdt;
    uint8_t reserved;
    uint8_t PreferredPMProfile;
    uint16_t SCI_Interrupt;
    uint32_t SMI_CommandPort;
    uint8_t AcpiEnable;
    uint8_t AcpiDisable;
    uint8_t S4BIOS_REQ;
    uint8_t PSTATE_Control;
    uint32_t PM1aEventBlock;
    uint32_t PM1bEventBlock;
    uint32_t PM1aControlBlock;
    uint32_t PM1bControlBlock;
    uint32_t PM2ControlBlock;
    uint32_t PMTimerBlock;
    uint32_t GPE0Block;
    uint32_t GPE1Block;
    uint8_t PM1EventLength;
    uint8_t PM1ControlLength;
    uint8_t reserved2[2];
    uint64_t X_FirmwareCtrl;
    uint64_t X_Dsdt;
    GAS X_PM1aControlBlock;
    GAS X_PM1bControlBlock;
};

// ===================== //
// HELPERS               //
// ===================== //

static void* phys_to_virt(uint64_t addr) 
{
    if (!hhdm_request.response) return (void*)addr;
    return (void*)(addr + hhdm_request.response->offset);
}

static bool acpi_sig(const char* a, const char* b) 
{
    for (int i = 0; i < 4; i++) 
    {
        if (a[i] != b[i])
        {
            return false;
        }
    }
    return true;
}

// ===================== //
// ACPI ENABLE           //
// ===================== //

static int acpiEnable(void) 
{
    if (PM1a_CNT != 0 && (inw((uint16_t)PM1a_CNT) & SCI_EN) != 0) 
    {
        return 0;
    }

    if (SMI_CMD != 0 && ACPI_ENABLE != 0) 
    {
        outb((uint16_t)SMI_CMD, ACPI_ENABLE);
        
        int i;
        for (i = 0; i < 300; i++) 
        {
            if ((inw((uint16_t)PM1a_CNT) & SCI_EN) == 1)
                break;
            for (int j = 0; j < 1000; j++) io_wait();
        }
        if (i < 300) return 0;
    }
    return 0; 
}

// ===================== //
// TABLE SEARCH          //
// ===================== //

static void* find_table(ACPISDTHeader* root, const char* name) 
{
    bool xsdt = acpi_sig(root->Signature, "XSDT");
    bool rsdt = acpi_sig(root->Signature, "RSDT");
    if (!xsdt && !rsdt) return nullptr;

    uint32_t size = root->Length - sizeof(ACPISDTHeader);
    uint32_t entries = xsdt ? (size / 8) : (size / 4);
    uint8_t* ptr = (uint8_t*)root + sizeof(ACPISDTHeader);

    for (uint32_t i = 0; i < entries; i++) 
    {
        uint64_t addr = xsdt ? ((uint64_t*)ptr)[i] : ((uint32_t*)ptr)[i];
        if (addr == 0) continue;

        ACPISDTHeader* table = (ACPISDTHeader*)phys_to_virt(addr);
        if (acpi_sig(table->Signature, name)) return table;
    }

    return nullptr;
}

// ===================== //
// AML PARSER (NOWE)     //
// ===================== //

// Funkcja pomocnicza: inteligentnie odczytuje wartości z języka AML (ACPI)
static uint16_t parse_aml_element(uint8_t** ptr) 
{
    uint8_t op = **ptr;
    (*ptr)++;
    if (op == 0x0A) 
    { // BytePrefix (Używane przez QEMU)
        uint16_t val = **ptr;
        (*ptr)++;
        return val;
    } 
    else if (op == 0x0B) 
    { // WordPrefix
        uint16_t val = *(uint16_t*)(*ptr);
        (*ptr) += 2;
        return val;
    } 
    else if (op == 0x00) 
    { // ZeroOp (Używane przez VirtualBox!)
        return 0;
    } 
    else if (op == 0x01) 
    { // OneOp
        return 1;
    } 
    else if (op == 0xFF) 
    { // OnesOp
        return 0xFFFF;
    }
    return 0; // Fallback
}

// Funkcja wyciągająca wartości do zamknięcia komputera (_S5_)
static bool parse_s5_from_aml(uint8_t* aml, uint32_t length) 
{
    if (length < 7) return false;

    for (uint32_t i = 0; i < length - 7; i++) 
    {
        // Szukamy sygnatury "_S5_"
        if (aml[i] == '_' && aml[i+1] == 'S' && aml[i+2] == '5' && aml[i+3] == '_') 
        {
            if (aml[i+4] == 0x12) 
            { // Sprawdzamy czy zaraz po nim jest PackageOp
                uint8_t* ptr = &aml[i+5]; // Wskaźnik na długość paczki (PkgLength)

                uint8_t pkg_lead = *ptr;
                ptr++;
                uint8_t len_bytes = (pkg_lead >> 6); // Ile dodatkowych bajtów definiuje długość?
                if (len_bytes > 0) 
                {
                    ptr += len_bytes; // Pomijamy bajty długości
                }

                ptr++; // Pomijamy NumElements (liczbę elementów)

                // Teraz wskaźnik idealnie trafia w wartości SLP_TYPa i SLP_TYPb!
                SLP_TYPa = parse_aml_element(&ptr) << 10;
                SLP_TYPb = parse_aml_element(&ptr) << 10;

                SLP_EN = 1 << 13;
                SCI_EN = 1;
                return true;
            }
        }
    }
    return false;
}

// ===================== //
// ACPI INIT (POPRAWIONE)//
// ===================== //

bool acpi_init() 
{
    if (!rsdp_request.response || !hhdm_request.response) 
    {
        return false;
    }

    struct limine_rsdp_response* rsdp_res = rsdp_request.response;
    RSDP* rsdp = (RSDP*)rsdp_res->address;
    if (!rsdp) return false;

    ACPISDTHeader* root;
    bool xsdt = false;
    if (rsdp->revision >= 2 && rsdp->xsdt_address) 
    {
        root = (ACPISDTHeader*)phys_to_virt(rsdp->xsdt_address);
        xsdt = true;
    } 
    else 
    {
        root = (ACPISDTHeader*)phys_to_virt(rsdp->rsdt_address);
    }

    if (!root) 
    {
        return false;
    }

    FADT* facp = (FADT*)find_table(root, "FACP");
    if (!facp) 
    {
        return false;
    }

    // Pobranie portów I/O
    if (facp->header.Length >= 148 && facp->X_PM1aControlBlock.Address != 0 && facp->X_PM1aControlBlock.AddressSpace == 0) 
    {
        PM1a_CNT = (uint32_t)facp->X_PM1aControlBlock.Address;
    } 
    else 
    {
        PM1a_CNT = facp->PM1aControlBlock;
    }

    if (facp->header.Length >= 160 && facp->X_PM1bControlBlock.Address != 0 && facp->X_PM1bControlBlock.AddressSpace == 0) 
    {
        PM1b_CNT = (uint32_t)facp->X_PM1bControlBlock.Address;
    } 
    else 
    {
        PM1b_CNT = facp->PM1bControlBlock;
    }

    PM1a_EVT = facp->PM1aEventBlock;
    SMI_CMD = facp->SMI_CommandPort;
    ACPI_ENABLE = facp->AcpiEnable;

    // FIX: Krytyczne zabezpieczenie. Jeśli tablica FADT jest krótka (jak w VirtualBox),
    // próba odczytu X_Dsdt powoduje odczyt złej pamięci i zawieszenie (crash) systemu!
    uint64_t dsdt_addr = (facp->header.Length >= 148 && facp->X_Dsdt != 0) ? facp->X_Dsdt : facp->Dsdt;
    
    // --- ETAP 1: Szukamy _S5_ w głównej tablicy DSDT ---
    uint8_t* dsdt = (uint8_t*)phys_to_virt(dsdt_addr);
    if (dsdt) 
    {
        uint32_t dsdtLength = ((ACPISDTHeader*)dsdt)->Length - sizeof(ACPISDTHeader);
        if (parse_s5_from_aml(dsdt + sizeof(ACPISDTHeader), dsdtLength)) 
        {
            return true; // Znaleziono w DSDT!
        }
    }

    // --- ETAP 2: Przeszukujemy poboczne tablice SSDT ---
    uint32_t size = root->Length - sizeof(ACPISDTHeader);
    uint32_t entries = xsdt ? (size / 8) : (size / 4);
    uint8_t* ptr_root = (uint8_t*)root + sizeof(ACPISDTHeader);

    for (uint32_t i = 0; i < entries; i++) 
    {
        uint64_t addr = xsdt ? ((uint64_t*)ptr_root)[i] : ((uint32_t*)ptr_root)[i];
        if (addr == 0) continue;

        ACPISDTHeader* table = (ACPISDTHeader*)phys_to_virt(addr);
        if (acpi_sig(table->Signature, "SSDT")) 
        {
            uint32_t ssdtLength = table->Length - sizeof(ACPISDTHeader);
            if (parse_s5_from_aml((uint8_t*)table + sizeof(ACPISDTHeader), ssdtLength)) 
            {
                return true; // Znaleziono _S5_ w SSDT!
            }
        }
    }

    return false;
}

// ===================== //
// SHUTDOWN              //
// ===================== //

void acpi_shutdown() 
{
    print_warn("NasuaOS: Closing system\n");
    print("ACPI: Initializing tables\n");

    if (!acpi_init()) 
    {
        print_error("ACPI: Not found _S5_ object. Stoping cpu.\n");
        asm volatile("cli");
        while (true) 
        { 
            asm volatile("hlt"); 
        }
    }

    print("ACPI: Found _S5_ objects. Activating power mode\n");
    
    acpiEnable();
    asm volatile("cli");

    if (PM1a_EVT != 0) 
    {
        outw((uint16_t)PM1a_EVT, 0xFFFF);
    }

    print("ACPI: Sending shutdown signal to IO\n");

    outw((uint16_t)PM1a_CNT, SLP_TYPa | SLP_EN);
    if (PM1b_CNT != 0) 
    {
        outw((uint16_t)PM1b_CNT, SLP_TYPb | SLP_EN);
    }

    while (true) 
    {
        asm volatile("hlt");
    }
}

// ===================== //
// REBOOT                //
// ===================== //

void acpi_reboot() 
{
    asm volatile("cli");
    outb(0x64, 0xFE);
    while (true) 
    {
        asm volatile("hlt");
    }
}
