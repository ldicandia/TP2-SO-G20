GLOBAL cpuVendor
GLOBAL getSeconds
GLOBAL getMinutes
GLOBAL getHours
GLOBAL spkIn
GLOBAL spkOut
GLOBAL acquire
GLOBAL release
GLOBAL _xadd
GLOBAL _xchg


section .text
	
cpuVendor:
	push rbp
	mov rbp, rsp

	push rbx

	mov rax, 0
	cpuid


	mov [rdi], ebx
	mov [rdi + 4], edx
	mov [rdi + 8], ecx

	mov byte [rdi+13], 0

	mov rax, rdi

	pop rbx

	mov rsp, rbp
	pop rbp
	ret

getSeconds:
	push rbp
	mov rbp, rsp

	mov al, 0x00
	out 70h, al
	in al, 71h

	movzx rax, al

	mov rsp, rbp
	pop rbp
	ret

getMinutes:
	push rbp
	mov rbp, rsp

	mov al, 0x02
	out 70h, al
	in al, 71h

	movzx rax, al

	mov rsp, rbp
	pop rbp
	ret

getHours:
	push rbp
	mov rbp, rsp

	mov al, 0x04
	out 70h, al
	in al, 71h

	movzx rax, al

	mov rsp, rbp
	pop rbp
	ret

spkIn:
	push rbp
	mov rbp, rsp

	mov rdx, rdi
	in al, dx

	mov rsp, rbp
	pop rbp
	ret

spkOut:
	push rbp
	mov rbp, rsp 

	mov rax, rsi 
	mov rdx, rdi 
	out dx, al 

	mov rsp, rbp
	pop rbp
	ret

acquire:
    mov al, 1
.spin:
    pause                   
    xchg al, [rdi]          
    test al, al
    jnz .spin
    ret

release:
    sfence                  
    mov byte [rdi], 0       
    ret

_xadd:
    mov rax, rsi            
    lock xadd [rdi], rax    
    ret

_xchg:
    mov rax, rsi            
    lock xchg [rdi], rax   
    ret

