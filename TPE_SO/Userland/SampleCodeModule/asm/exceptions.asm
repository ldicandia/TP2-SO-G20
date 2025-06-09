GLOBAL u_exc_invopcode
GLOBAL u_exc_zerodiv

section .text

u_exc_invopcode:
	mov cr6, rax
	ret

u_exc_zerodiv:
	mov rax, 0
	div rax
	ret

section .bss