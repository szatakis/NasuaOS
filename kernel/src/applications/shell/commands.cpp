#include <cstddef> // Definiuje size_t
#include <cstdint> // DODANE: Definiuje uint32_t dla kolorów HEX
#include <stdint.h>
#include <stddef.h>
#include <limine.h>

#include "commands.h"
#include "libs/libc/libc.h"
#include "screen.h"

#include "applications/applications.h"

#include "system/drivers/drivers.h"
#include "system/vars/info_vars/info_vars.h"

#include "kernel/include/panic/kernel_panic.h"
#include "kernel/include/logger/logger.hpp"

#include "system/filesystem/ztrfs.h"

#include "libs/asm/asm.h"

extern uint32_t current_text_color;
extern bool debug_mode;

static LogLevel parse_log_level(const char* level)
{
    if(shell_strncmp(level, "INFO", 4))
        return INFO;

    if(shell_strncmp(level, "WARN", 4))
        return WARN;

    if(shell_strncmp(level, "ERROR", 5))
        return ERROR;

    if(shell_strncmp(level, "DEBUG", 5))
        return DEBUG;


    return INFO;
}

// --- GŁÓWNA FUNKCJA SHELLA ---

void execute_command(const char *cmd) {

    print("\n");
    
    if (cmd == nullptr || *cmd == '\0') {
        print_cmd();
        return;
    }

    const char* space_ptr = shell_strchr(cmd, ' ');
    size_t cmd_name_len = space_ptr ? (space_ptr - cmd) : shell_strlen(cmd);
    const char* args = space_ptr ? (space_ptr + 1) : "";

    char single_char_buf[2] = {0, 0};

    print(CMD_TEXT_WHITE);
    // komenda: help
    if (cmd_name_len == 4 && shell_strncmp(cmd, "help", 4)) {

        const char* arg_check = args;

        while (*arg_check == ' ')
            arg_check++;

        if (*arg_check != '\0') {

            if (!shell_strncmp(arg_check, "--page ", 7)) {
                print_error("Syntax error!\n");
                print_info("Usage: help --page [1-10]\n");
                print_cmd();
                return;
            }
        }

        int page = 1; // domyślna strona

        const char* page_flag = shell_strstr(args, "--page ");

        if (page_flag) {
            const char* page_num = page_flag + 7;

            page = parse_number(page_num);

            if (page < 1 || page > 10) {
                print_error("Syntax error!\n");
                print_info("Usage: help --page [1-10]\n");
                print_cmd();
                return;
            }
        }

        if (page == 1) {
            print_info("Available commands (Page 1/10):\n");
            print(" -help                               - Show the first page of help\n");
            print("   --page [1-10]                     - Show specific help page\n");
            print(" -info                               - Display OS version and hardware info\n");
            print(" -source                             - Show link to the OS source code\n");
            print(" -shutdown                           - Power off the system safely\n");
            print(" -reboot                             - Restart the computer\n");
            print(" -echo                               - Print text to the screen or write to file\n");
            print("   --text \"text\"                     - (Required) Specify the text to print\n");
            print("   --color \"0xRRGGBB\"                - (Optional) Set custom HEX text color\n");
            print("   --file \"file_name.file_ext\"       - (Optional) Save the text output into a file\n");
            print(" -debug                              - Enable or disable debug mode\n");
            print("   --on                              - (Required 1 of 2) Enable debug mode\n");
            print("   --off                             - (Required 1 of 2) Disable debug mode\n");
        } 

        else if (page == 2) {
            print_info("Available commands (Page 2/10):\n");
            print(" -clear                              - Clear the terminal screen\n");
            print(" -fetch                              - Display system summary and ASCII logo\n");
            print(" -format                             - Format storage drive\n");
            print(" -dir                                - List files and directories in current path\n");
            print(" -touch                              - Create a new empty file\n");
            print("   --file \"file_name.file_ext\"       - (Required) Name of the file to create\n");
            print(" -mount                              - Mount a filesystem or device\n");
            print(" -uart                               - Send data over serial port\n");
            print("   --text \"text\"                     - (Required) Text string to transmit via UART\n");
            print(" -time                               - System clock utility\n");
            print("   --get                             - (Required 1 of 3) Display current date and time\n");
            print("   --set \"DD.MM.YYYY HH:MM:SS\"       - (Required 1 of 3) Set new system date and time\n");
            print("   --info                            - (Required 1 of 3) Display CMOS storage and RTC battery status\n");
        }

        else if (page == 3) {
            print_info("Available commands (Page 3/10):\n");
            print(" -uptime                             - Display system uptime since boot\n");
            print(" -panic                              - Trigger kernel panic for debugging\n");
            print(" -resolution                         - Display current screen resolution and video mode information\n");
            print(" -logs                               - Kernel log management utility\n");
            print("    --show                           - (Required 1 of 5) Display stored kernel logs from memory\n");
            print("    --clear                          - (Required 1 of 5) Clear kernel log buffer\n");
            print("    --level \"INFO|WARN|ERROR|DEBUG\"  - (Required 1 of 5) Display only selected level messages\n");
            print("    --subsystem \"subsystem_name\"     - (Required 1 of 5) Display logs from selected subsystem\n");
            print("    --put \"text\"                     - (Required 1 of 5) Add custom log message with INFO level\n");
            print(" -bootapp                            - Application manager command\n");
            print("    --app \"app_name\"                 - (Required) Load and execute selected application\n");
        }

        else if (page == 4) {
            print_info("Available commands (Page 4/10):\n");
            print("More commands coming soon...\n");
        }

        else if (page == 5) {
            print_info("Available commands (Page 5/10):\n");
            print("More commands coming soon...\n");
        }

        else if (page == 6) {
            print_info("Available commands (Page 6/10):\n");
            print("More commands coming soon...\n");
        }

        else if (page == 7) {
            print_info("Available commands (Page 7/10):\n");
            print("More commands coming soon...\n");
        }

        else if (page == 8) {
            print_info("Available commands (Page 8/10):\n");
            print("More commands coming soon...\n");
        }

        else if (page == 9) {
            print_info("Available commands (Page 9/10):\n");
            print("More commands coming soon...\n");
        }

        else if (page == 10) {
            print_info("Available commands (Page 10/10):\n");
            print("More commands coming soon...\n");
        }
    }
    // 2. Komenda: clear
    else if (cmd_name_len == 5 && shell_strncmp(cmd, "clear", 5)) {
        clear_screen();

        update_bottom_bar();
        draw_mouse_cursor();

        fetch();

        print(CMD_TEXT_WHITE);
        print("Enter Command\n");

        print_cmd();

        return; 
    } 
    // 3. Komenda: echo
    else if (cmd_name_len == 4 && shell_strncmp(cmd, "echo", 4)) {
        // Szukamy parametru --text w dowolnym miejscu argumentów
        const char* text_flag = shell_strstr(args, "--text ");
        
        if (text_flag) {
            const char* text_content = text_flag + 7; // Przesunięcie za "--text "
            
            // Zapisujemy oryginalny kolor, aby przywrócić go po zakończeniu polecenia echo
            uint32_t old_color = current_text_color;

            // Szukamy parametru --color w argumentach
            const char* color_flag = shell_strstr(args, "--color ");
            if (color_flag) {
                const char* color_val = color_flag + 8; // Przesunięcie za "--color "
                // Parsujemy podany przez użytkownika kolor HEX i ustawiamy go w systemie graficznym
                current_text_color = parse_hex_color(color_val);
            }

        const char* file_flag = shell_strstr(args, "--file ");
        if (file_flag) {
            const char* file_name = file_flag + 7;
    
            // Bufor na nową nazwę pliku
            char file_name_n[128] = {0}; 

            // Sprawdź, czy nazwa zaczyna się od cudzysłowu
            if (file_name[0] == '"') {
                file_name++; // Przesuń początek za pierwszy cudzysłów

                // Znajdź końcowy cudzysłów lub koniec stringa
                size_t len = 0;
                while (file_name[len] != '\0' && file_name[len] != '"') {
                    len++;
                }

                // Ręczne kopiowanie znaków zamiast strncpy
                if (len < sizeof(file_name_n)) {
                    for (size_t i = 0; i < len; i++) {
                        file_name_n[i] = file_name[i];
                    }
                    file_name_n[len] = '\0'; // Bezpieczne domknięcie stringa
                }
            } else {
                // Jeśli nie ma cudzysłowów, kopiuj do spacji lub końca stringa
                size_t len = 0;
                while (file_name[len] != '\0' && file_name[len] != ' ') {
                    len++;
                }

                if (len < sizeof(file_name_n)) {
                    for (size_t i = 0; i < len; i++) {
                        file_name_n[i] = file_name[i];
                    }
                    file_name_n[len] = '\0';
                }
            }

            ztrfs_create_file(file_name_n);

            print_info("Saved to file ");
            print(file_name_n);
            print(" with text ");
        }


            // Wypisywanie tekstu
            if (*text_content == '"') {
                text_content++; 
                while (*text_content && *text_content != '"') {
                    single_char_buf[0] = *text_content;
                    print(single_char_buf);
                    text_content++;
                }
            } else {
                // Jeśli brak cudzysłowów, wypisujemy do momentu napotkania kolejnej flagi '-' lub końca
                while (*text_content && *text_content != '-') {
                    single_char_buf[0] = *text_content;
                    print(single_char_buf);
                    text_content++;
                }
            }
            print("\n");

            // Przywracamy domyślny kolor tekstu (żeby następne komendy nie pisały w HEX)
            current_text_color = old_color;
        } else {
            print_error("Syntax error!\n");
            print_info("Usage: echo --text \"your text\" --color \"0xRRGGBB\"\n");
        }
    } 
    // 4.komenda: fetch
    else if (cmd_name_len == 5 && shell_strncmp(cmd, "fetch", 5)) {
        fetch();
    }
    // 5. komenda: time
    else if (cmd_name_len == 4 && shell_strncmp(cmd, "time", 4)) {
        // Sprawdzamy, czy użytkownik podał flagę --set lub --get
        const char* set_flag = shell_strstr(args, "--set ");
        const char* get_flag = shell_strstr(args, "--get");
        const char* status_flag = shell_strstr(args, "--info");

        // Opcja A: ZAPIS CZASU (--set)
        if (set_flag) {
            // Oczekiwany format po "--set ": "DD.MM.YYYY HH:MM:SS"
            // set_flag + 6 wskazuje na początek daty
            const char* date_ptr = set_flag + 6;

            // Walidacja długości (DD.MM.YYYY HH:MM:SS to dokładnie 19 znaków)
            if (shell_strlen(date_ptr) < 19 || date_ptr[2] != '.' || date_ptr[5] != '.' || date_ptr[10] != ' ' || date_ptr[13] != ':' || date_ptr[16] != ':') {
                print_error("Syntax error!\n");
                print_info("Usage: time --set DD.MM.YYYY HH:MM:SS\n");
            } else {
                RtcTime new_time;
                new_time.day    = parse_digits(date_ptr, 2);
                new_time.month  = parse_digits(date_ptr + 3, 2);
                new_time.year   = parse_digits(date_ptr + 6, 4);
                new_time.hour   = parse_digits(date_ptr + 11, 2);
                new_time.minute = parse_digits(date_ptr + 14, 2);
                new_time.second = parse_digits(date_ptr + 17, 2);

                // Zapisujemy czas do kości RTC
                set_rtc_time(new_time);
                print_info("System time updated successfully!\n");
            }
        }
        // Opcja B: ODCZYT CZASU (wymaga jawnego --get)
        else if (get_flag) {
            RtcTime time = get_rtc_time();
            
            // Wyświetlanie daty: DD.MM.YYYY
            print_num_padded(time.day);
            print(".");
            print_num_padded(time.month);
            print(".");
            print_year(time.year);

            print(" "); // Odstęp

            // Wyświetlanie czasu: HH:MM:SS
            print_num_padded(time.hour);
            print(":");
            print_num_padded(time.minute);
            print(":");
            print_num_padded(time.second);
            print("\n");
        }
        else if (status_flag) {
            if (!is_rtc_battery_ok()) {
                print_warn("CMOS battery is dead. System time may be incorrect.\n");
            } else {
                print_info("CMOS battery is working properly.\n");
            }
        }
        // Obsługa braku argumentów lub niepoprawnych flag (np. "time", "time --byleco")
        else {
            print_error("Syntax error!\n");
            print("Usage:\n");
            print("  time --get   - display current time\n");
            print("  time --set DD.MM.YYYY HH:MM:SS - set new time\n");
            print("  time --info - Checks RTC battery status\n");
        }
    }
    // 6. komenda: reboot
    else if (cmd_name_len == 6 && shell_strncmp(cmd, "reboot", 6)) {
        acpi_reboot();
    }
    // 7. komenda: shutdown
    else if (cmd_name_len == 8 && shell_strncmp(cmd, "shutdown", 8)) {
        acpi_shutdown();
    }
    // 8. komenda: format
    else if(cmd_name_len == 6 && shell_strncmp(cmd,"format", 6)){
        print_warn("Formatting ZTRFS...\n");

        ztrfs_format();

        print_info("Done.\n");
    }
    // 9. komenda: dir
    else if(cmd_name_len == 3 && shell_strncmp(cmd, "dir", 3)){
        ztrfs_dir();
    }
    // 10. komenda: mount
    else if(cmd_name_len == 5 && shell_strncmp(cmd, "mount", 5)) {
        ztrfs_exists();
    }
    // 11. komenda: touch
    else if (cmd_name_len == 5 && shell_strncmp(cmd, "touch", 5)) {
        // Szukamy parametru --filename w argumentach
        const char* filename_flag = shell_strstr(args, "--file ");

        if (filename_flag) {
            const char* filename_ptr = filename_flag + 7; // Przesunięcie za "--file "
            char name_buf[64]; // Bufor na wyciągniętą nazwę pliku
            int i = 0;

            // Obsługa nazwy w cudzysłowie
            if (*filename_ptr == '"') {
                filename_ptr++; // Pomiń początkowy cudzysłów
                while (*filename_ptr && *filename_ptr != '"' && i < 63) {
                    name_buf[i++] = *filename_ptr++;
                }
            } 
            // Obsługa nazwy bez cudzysłowów (do spacji, kolejnej flagi lub końca)
            else {
                while (*filename_ptr && *filename_ptr != ' ' && *filename_ptr != '-' && i < 63) {
                    name_buf[i++] = *filename_ptr++;
                }
            }
            name_buf[i] = '\0'; // Zamknięcie ciągu znaków

            // Sprawdzamy czy nazwa pliku nie jest pusta
            if (i > 0) {
                ztrfs_create_file(name_buf);
            } else {
                print_error("Error: Filename cannot be empty!\n");
            }
        } else {
            print_error("Syntax error!\n");
            print_info("Usage: touch --file \"name.file_extension\"\n");
        }
    }
    // 12. komenda: info
    else if (cmd_name_len == 4 && shell_strncmp(cmd, "info", 4)) {
        char cpu_name[49];
        char buf[32];
        // Wersja DLA TEKSTU (używana do CPU)
        auto print_line = [&](const char* label, const char* value) {
            print(CMD_TEXT_WHITE);
            print(label);
            print(CMD_TEXT_GRAY);
            print(value);
            print("\n");
        };

        auto print_line_mb = [&](const char* label, const char* value) {
            print(CMD_TEXT_WHITE);
            print(label);
            print(CMD_TEXT_GRAY);
            print(value);
            print("MB");
            print("\n");
        };

        // Wersja DLA LICZBY (używana do RAMu i Dysku)
        auto print_numeric = [&](const char* label, uint64_t bytes) {
            uint64_t mb = bytes / 1024 / 1024;
            itoa(mb, buf);
        
            print(CMD_TEXT_WHITE);
            print(label);
            print(CMD_TEXT_GRAY);
            print(buf);
            print("MB\n");
        };
    
        cpu_get_brand(cpu_name);
        storage_init(); // Uwaga: Docelowo przenieś do kernel_main

        // 1. Sekcja: Software
        print_info("Software information\n");
        print_line("System Version: ", "NasuaOS 0.0.3");
        print_line("Kernel Version: ", "0.0.1\n\n");

        print_info("Hardware information\n");

        // 2. Sekcja: Hardware (wszystko wyświetlane jednolicie)
        print_line("CPU:            ", cpu_name);
        print_line_mb("Total RAM:      ", memory_total());
        print_numeric("Used RAM:       ", memory_used());
        print_numeric("Total Storage:  ", storage_total());
        print_numeric("Used Storage:   ", storage_used());

        // Przywrócenie domyślnego koloru konsoli na końcu
        print(CMD_TEXT_WHITE);
    }
    // 13. komenda: source
    else if (cmd_name_len == 6 && shell_strncmp(cmd, "source", 6)) {
        print_info("Source anvible at: https://github.com/szatakis/NasuaOS \n");
    }
    // 14. komenda: uart
    else if (cmd_name_len == 4 && shell_strncmp(cmd, "uart", 4)) {
        const char* text_flag = shell_strstr(args, "--text ");

        // Wymagaj flagi --text. Jeśli args jest puste lub nie ma poprawnej flagi -> błąd
        if (shell_strlen(args) == 0 || !text_flag) {
            print_error("Syntax error! Argument '--text' is required.\n");
            print_info("Usage: uart --text \"text\"\n");
        }
        else {
            const char* text = text_flag + 7; // za "--text "

            if (*text == '"') {
                text++; // pomijamy pierwszy cudzysłów

                char buf[128];
                size_t i = 0;

                while (*text && *text != '"' && i < sizeof(buf) - 1) {
                    buf[i++] = *text++;
                }

                buf[i] = '\0';
                
                Uart::puts(buf);
                Uart::puts("\n");
            } 
            else {
                Uart::puts(text);
                Uart::puts("\n");
            }
        }
    }
    // 15. komenda: debug
    else if (cmd_name_len == 5 && shell_strncmp(cmd, "debug", 5)) {
        if (shell_strlen(args) == 0) {
            print_error("Syntax error! Argument required.\n");
            print_info("Usage: debug --on | --off\n");
        }
        else if (shell_strncmp(args, "--on", 4)) {
            debug_mode = true;

            print_info("Debug mode ON\n");
            Uart::puts("Debug ON\n");
        }
        else if (shell_strncmp(args, "--off", 5)) {
            debug_mode = false;

            print_info("Debug mode OFF\n");
            Uart::puts("Debug OFF\n");
        }
        else {
            print_error("Syntax error! Unknown argument.\n");
            print_info("Usage: debug --on | --off\n");
        }
    }
    // 16. komenda: resolution
    else if (cmd_name_len == 10 && shell_strncmp(cmd, "resolution", 10)) {
        print_resolution();
    }
    // 17. komenda: uptime
    else if (cmd_name_len == 6 && shell_strncmp(cmd, "uptime", 6)) {
        print_uptime();
    }
    // 18. komenda: panic
    else if (cmd_name_len == 5 && shell_strncmp(cmd, "panic", 5)) {
        kernel_panic("Manual panic triggered by user");
    }
    // 19. komenda: logs
    else if (cmd_name_len == 4 && shell_strncmp(cmd, "logs", 4)) {
        // logs bez argumentów
        if(shell_strstr(args, "--show"))
        {
            print_logs();
        }

        // logs --clear
        else if(shell_strstr(args, "--clear"))
        {
            clear_logs();

            print_info("Kernel log buffer cleared\n");
        }

        // logs --put "text" (automatycznie: INFO, USER)
        else if(shell_strstr(args, "--put "))
        {
            const char* put_arg = shell_strstr(args, "--put ") + 6;

            char text[128]; // Maksymalna długość wiadomości tekstowej
            int i = 0;

            // Ominięcie otwierającego cudzysłowu jeśli istnieje
            if(put_arg[0] == '"') {
                put_arg++;
            }

            // Wyciąganie wiadomości tekstowej aż do zamknięcia cudzysłowu lub końca stringa
            while(put_arg[i] != '"' && put_arg[i] != '\0' && i < 127) {
                text[i] = put_arg[i];
                i++;
            }
            text[i] = '\0';

            // Sprawdzenie poprawności i wywołanie funkcji docelowej
            if(shell_strlen(text) > 0) {
                log(parse_log_level("INFO"), "USER", text);
                print_info("Log added successfully\n");
            } else {
                print_error("Invalid --put format! Use: logs --put \"your message\"\n");
            }
        }

        // logs --level "ERROR"
        else if(shell_strstr(args, "--level "))
        {
            const char* level_arg = shell_strstr(args, "--level ") + 8;

            char level_name[16];
            int i = 0;

            // obsługa cudzysłowów
            if(level_arg[0] == '"')
            {
                level_arg++;

                while(level_arg[i] != '"' &&
                      level_arg[i] != '\0' &&
                      i < 15)
                {
                    level_name[i] = level_arg[i];
                    i++;
                }
            }
            else
            {
                while(level_arg[i] != ' ' &&
                      level_arg[i] != '\0' &&
                      i < 15)
                {
                    level_name[i] = level_arg[i];
                    i++;
                }
            }

            level_name[i] = '\0';

            print_logs_level(parse_log_level(level_name));
        }

        // logs --subsystem "USB"
        else if(shell_strstr(args, "--subsystem "))
        {
            const char* subsystem_arg = shell_strstr(args, "--subsystem ") + 12;

            char subsystem[32];
            int i = 0;

            if(subsystem_arg[0] == '"')
            {
                subsystem_arg++;

                while(subsystem_arg[i] != '"' &&
                    subsystem_arg[i] != '\0' &&
                    i < 31)
                {
                    subsystem[i] = subsystem_arg[i];
                    i++;
                }
            }
            else
            {
                while(subsystem_arg[i] != ' ' &&
                      subsystem_arg[i] != '\0' &&
                      i < 31)
                {
                    subsystem[i] = subsystem_arg[i];
                    i++;
                }
            }

            subsystem[i] = '\0';

            print_logs_subsystem(subsystem);
        }


        else
        {
            print_error("Syntax error!\n");

            print_info("Usage:\n");
            print("  logs\n");
            print("  logs --clear\n");
            print("  logs --level \"INFO|WARN|ERROR|DEBUG\"\n");
            print("  logs --subsystem \"name\"\n");
            print("  logs --put \"your message\"\n");
        }
    }
    // 20. komenda: bootapp
    else if (cmd_name_len == 7 && shell_strncmp(cmd, "bootapp", 7)) {

        const char* app_flag = shell_strstr(args, "--app ");

        if (app_flag) {
            const char* app_name = app_flag + 6; // po "--app "

            if (*app_name == '"') {
                app_name++;

                char app_name_buf[64] = {0};

                size_t len = 0;
                while (app_name[len] != '\0' && app_name[len] != '"') {
                    len++;
                }

                if (len < sizeof(app_name_buf)) {
                    for (size_t i = 0; i < len; i++) {
                        app_name_buf[i] = app_name[i];
                    }

                    app_name_buf[len] = '\0';
                }

                if (shell_strncmp(app_name_buf, "terminal", 8)) {
                    draw_window(&terminal);
                    terminal.id = current_id;
                    current_id++;
                }
                if (shell_strncmp(app_name_buf, "zetedit", 7)) {
                    draw_window(&zetedit);
                    terminal.id = current_id;
                    current_id++;
                }
                else {
                    print_error("Unknown app\n");
                }
            }
        }
        else {
            print_error("Syntax error!\n");
            print_info("Usage: bootapp --app \"app_name\"\n");
        }
    }

    // Obsługa nieznanej komendy
    else {
        print_error("Unknown command: ");
        for (size_t i = 0; i < cmd_name_len; i++) {
            single_char_buf[0] = cmd[i];
            print(single_char_buf);
        }
        print("\n");
    }
    
    print_cmd();
}