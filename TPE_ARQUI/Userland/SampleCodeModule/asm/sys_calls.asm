GLOBAL u_sys_read
GLOBAL u_sys_write
GLOBAL u_sys_write_color
GLOBAL u_sys_clear
GLOBAL u_sys_getHours
GLOBAL u_sys_getMinutes
GLOBAL u_sys_getSeconds
GLOBAL u_sys_increment_size
GLOBAL u_sys_decrement_size
GLOBAL u_sys_infoReg
GLOBAL u_sys_drawSquare
GLOBAL u_sys_sleep
GLOBAL u_sys_playSound
GLOBAL u_sys_stopSound

section .text

u_sys_read:
    mov rax, 0x00
    int 0x80
    ret

u_sys_write:
    mov rax, 0x01
    int 0x80
    ret

u_sys_write_color:
    mov rax, 0x02
    int 0x80
    ret

u_sys_clear:
    mov rax, 0x03
    int 0x80
    ret

u_sys_increment_size:
    mov rax, 0x04
    int 0x80
    ret

u_sys_decrement_size:
    mov rax, 0x05
    int 0x80
    ret

u_sys_getHours:
    mov rax, 0x06
    int 0x80
    ret 
u_sys_getMinutes:
    mov rax, 0x07
    int 0x80
    ret
u_sys_getSeconds:
    mov rax, 0x08
    int 0x80
    ret

u_sys_infoReg:
    mov rax, 0x09
    int 0x80
    ret

u_sys_drawSquare:
    mov rax, 0x0A
    int 0x80
    ret

u_sys_sleep: 
    mov rax, 0x0B
    int 0x80
    ret

u_sys_playSound:
    mov rax, 0x0C
    int 0x80
    ret
    
u_sys_stopSound:
    mov rax, 0x0D
    int 0x80
    ret
