[bits 32]

section .multiboot
align 4

header:
    dd 0x1BADB002
    dd 0
    dd -(0x1BADB002)


section .text
global _start


_start:
    mov esp, stack_top

    call disable_cursor
    call keyboard_init
    call clear_screen

    mov esi,banner
    call print


shell:
    mov esi,prompt
    call print

    mov edi,buffer
    call read_line


    mov esi,buffer
    mov edi,cmd_help
    call strcmp
    cmp eax,1
    je help


    mov esi,buffer
    mov edi,cmd_clear
    call strcmp
    cmp eax,1
    je clear


    mov esi,buffer
    mov edi,cmd_info
    call strcmp
    cmp eax,1
    je info


    mov esi,buffer
    mov edi,cmd_halt
    call strcmp
    cmp eax,1
    je halt


    mov esi,unknown
    call print

    jmp shell



help:
    mov esi,help_msg
    call print
    jmp shell


clear:
    call clear_screen
    jmp shell



info:
    mov esi,info_msg
    call print
    jmp shell


halt:
    cli

.h:
    hlt
    jmp .h


; =========================
; KEYBOARD INIT
; =========================


keyboard_init:

.wait:
    in al,0x64
    test al,2
    jnz .wait


    mov al,0xAE
    out 0x64,al


.wait2:
    in al,0x64
    test al,2
    jnz .wait2


    mov al,0xF4
    out 0x60,al


    ret


; =========================
; READ LINE
; =========================


read_line:

.loop:
    in al,0x64
    test al,1
    jz .loop


    in al,0x60


    ; key release
    test al,0x80
    jnz .loop


    ; enter
    cmp al,0x1C
    je .done


    ; backspace
    cmp al,0x0E
    je .backspace



    movzx eax,al

    mov al,[keymap+eax]


    cmp al,0
    je .loop

    stosb

    mov ah,0x07

    mov ebx,[cursor]

    mov [0xB8000+ebx],ax

    add ebx,2

    mov [cursor],ebx


    jmp .loop


.backspace:
    cmp edi,buffer
    je .loop

    dec edi

    mov byte [edi],0


    sub dword [cursor],2


    mov ebx,[cursor]

    mov ax,0x0720

    mov [0xB8000+ebx],ax


    jmp .loop


.done:
    mov byte [edi],0

    call newline

    ret


; =========================
; VGA
; =========================
clear_screen:
    mov edi,0xB8000
    mov ecx,2000
    mov ax,0x0720

.loop:
    stosw
    loop .loop

    mov dword [cursor],0

    ret


disable_cursor:
    mov dx,0x3D4

    mov al,0x0A
    out dx,al

    inc dx

    mov al,0x20
    out dx,al

    ret


print:


.next:
    lodsb

    cmp al,0
    je .done


    cmp al,10
    je .newline


    mov ebx,[cursor]

    mov ah,0x07

    mov [0xB8000+ebx],ax

    add ebx,2

    mov [cursor],ebx


    jmp .next


.newline:
    call newline
    jmp .next


.done:
    ret


newline:
    mov eax,[cursor]

    mov edx,0
    mov ebx,160

    div ebx

    inc eax

    mul ebx


    mov [cursor],eax

    ret


; =========================
; STRING
; =========================
strcmp:

.loop:
    mov al,[esi]
    mov bl,[edi]


    cmp al,bl
    jne .no


    cmp al,0
    je .yes


    inc esi
    inc edi

    jmp .loop


.yes:

    mov eax,1
    ret


.no:

    xor eax,eax
    ret


; =========================
; DATA
; =========================
banner:
db "NasuaOS 32-bit",10,0


prompt:
db "> ",0


help_msg:
db "Commands:",10
db "help",10
db "clear",10
db "info",10
db "halt",10,10,0


info_msg:
db "Mode: protected mode",10
db "Kernel: NasuaOS 32-bit",10
db "Kernel 32-bit is basic not full os",10,10,0


unknown:
db "Unknown command",10,0


cmd_help:
db "help",0

cmd_clear:
db "clear",0

cmd_info:
db "info",0

cmd_halt:
db "halt",0



; =========================
; PS2 SCANCODE MAP
; =========================
keymap:
; 0x00 - 0x0F
db 0, 0x1B, "1234567890-="       ; 0x01 = Esc, next numbers
db 0x08, 0x09                    ; 0x0E = Backspace, 0x0F = Tab

; 0x10 - 0x1F
db "qwertyuiop[]"                ; Letters and brackets
db 0x0D, 0                       ; 0x1C = Enter, 0x1D = Left Ctrl

; 0x20 - 0x2F
db "asdfghjkl;'"                 ; Letters and punctuation marks
db "`", 0, "\"                   ; 0x29 = Backtick, 0x2A = Left Shift, 0x2B = Backslash

; 0x30 - 0x3F
db "zxcvbnm,./"                  ; LLetters and bottom row chars
db 0, "*", 0, " "                ; 0x36 = Right Shift, 0x37 = Keypad *, 0x38 = Left Alt, 0x39 = Spacja
db 0, 0                          ; 0x3A = CapsLock, 0x3B = F1

times 256-($-keymap) db 0


section .bss


cursor:
resd 1


buffer:
resb 128


align 16

stack:
resb 4096

stack_top: