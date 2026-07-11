#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>
#include <stdbool.h>


bool acpi_init();
void acpi_shutdown();
void acpi_reboot();


#endif