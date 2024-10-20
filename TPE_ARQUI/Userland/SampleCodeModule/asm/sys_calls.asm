GLOBAL u_sys_read
GLOBAL u_sys_write
GLOBAL u_sys_write_color

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
