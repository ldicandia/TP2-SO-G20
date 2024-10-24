global loader
global reset
extern main
extern initializeKernelBinary
extern getStackBase

loader:
	call initializeKernelBinary	; Set up the kernel binary, and get thet stack address
	mov rsp, rax				; Set up the stack with the returned address
	call main
hang:
	cli
	hlt	; halt machine should kernel return
	jmp hang
	

section .data
codeModule equ 0x400000