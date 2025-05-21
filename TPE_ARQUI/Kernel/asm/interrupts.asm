GLOBAL _cli
GLOBAL _sti
GLOBAL picMasterMask
GLOBAL picSlaveMask
GLOBAL haltcpu
GLOBAL _hlt

GLOBAL _interrupt_syscall
GLOBAL _interrupt_timerTick
GLOBAL _interrupt_keyboardHandler
GLOBAL _exception_divideByZero
GLOBAL _exception_invalidOpCode

GLOBAL _irq00handler
GLOBAL _irq01handler
GLOBAL _irq02handler
GLOBAL _irq03handler
GLOBAL _irq04handler
GLOBAL _irq05handler
GLOBAL forceTimerTick


GLOBAL _initialize_stack_frame


EXTERN getStackBase
EXTERN keyboard_master
EXTERN ctrl_c_handler
EXTERN timer_master
EXTERN sys_master
EXTERN exception_master
EXTERN irqDispatcher
EXTERN schedule

GLOBAL regdata_exc
GLOBAL hasInforeg
GLOBAL inforeg

SECTION .text

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

%macro pushStateNoRax 0
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popStateNoRax 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
%endmacro

%macro endOfHardwareInterrupt 0 ; para el keyboard y el timer tick
    mov al, 20h
    out 20h, al
%endmacro

%macro saveRegistersException 0
	mov [regdata_exc+0], 	rax ;0
	mov [regdata_exc+8], 	rbx ;1
	mov [regdata_exc+16], 	rcx ;2
	mov [regdata_exc+24], 	rdx ;3
	mov [regdata_exc+32], 	rsi ;4
	mov [regdata_exc+40], 	rdi ;5
	mov [regdata_exc+48], 	rbp ;6
	mov [regdata_exc+64], 	r8  ;8
	mov [regdata_exc+72], 	r9  ;9
	mov [regdata_exc+80], 	r10 ;10
	mov [regdata_exc+88], 	r11	;11
	mov [regdata_exc+96], 	r12 ;12
	mov [regdata_exc+104], 	r13 ;13
	mov [regdata_exc+112], 	r14 ;14
	mov [regdata_exc+120], 	r15 ;15


	mov rax, rsp 
	add rax, 40
	mov [regdata_exc+56], rax

	mov rax, [rsp]
	mov [regdata_exc+128], rax

	mov rax, [rsp+8]
	mov [regdata_exc+136], rax 
%endmacro

%macro exceptionHandler 1
	pushState

	mov rdi, %1
	call exceptionDispatcher

	popState
	iretq
%endmacro

%macro irqHandlerMaster 1
	pushState

	mov rdi, %1 ; pasaje de parametro
	call irqDispatcher

	; signal pic EOI (End of Interrupt)
	mov al, 20h
	out 20h, al

	popState
	iretq
%endmacro


_hlt:
	sti
	hlt
	ret

_cli:
	cli
	ret


_sti:
	sti
	ret

picMasterMask:
	push rbp
    mov rbp, rsp
    mov ax, di
    out	21h,al
    pop rbp
    retn

picSlaveMask:
	push    rbp
    mov     rbp, rsp
    mov     ax, di 
    out	0A1h,al
    pop     rbp
    retn

_interrupt_keyboardHandler:
    pushState

    xor rax, rax
    in al, 60h 	
    mov rdi, rax 

    ; Detect left shift press
    cmp al, 0x2A 	; left shift
    jne .checkLeftShiftRelease
    mov byte [left_shift], 1
    jmp .afterShift

.checkLeftShiftRelease:
    cmp al, 0xAA 	; left shift released
    jne .checkCtrl
    mov byte [left_shift], 0
    jmp .afterShift

.checkCtrl:
    cmp al, 0x1D     ; Ctrl press
    jne .checkCtrlRelease
    mov byte [ctrl_pressed], 1
    jmp .afterShift

.checkCtrlRelease:
    cmp al, 0x9D     ; Ctrl release
    jne .afterShift
    mov byte [ctrl_pressed], 0

.afterShift:

    ; Ctrl+C detection
    cmp al, 0x2E     ; 'C' key
    jne .checkTab
    cmp byte [ctrl_pressed], 1
    jne .checkTab

    ; Call ctrl_c_handler if Ctrl+C is pressed
    call ctrl_c_handler
    jmp .keyboardContinue

.checkTab:
    cmp byte [left_shift], 1 
    jne .keyboardContinue
    cmp al, 0x0F     ; 'TAB'
    jne .keyboardContinue

    ; Save registers to inforeg
    mov [inforeg+2*8], rbx
    mov [inforeg+3*8], rcx
    mov [inforeg+4*8], rdx
    mov [inforeg+5*8], rsi
    mov [inforeg+6*8], rdi
    mov [inforeg+7*8], rbp
    mov [inforeg+9*8], r8
    mov [inforeg+10*8], r9
    mov [inforeg+11*8], r10
    mov [inforeg+12*8], r11
    mov [inforeg+13*8], r12
    mov [inforeg+14*8], r13
    mov [inforeg+15*8], r14
    mov [inforeg+16*8], r15

    mov rax, rsp
    add rax, 160 
    mov [inforeg+8*8], rax 

    mov rax, [rsp+15*8]
    mov [inforeg], rax 
	
    mov rax, [rsp+14*8]
    mov [inforeg+1*8], rax 

    mov byte [hasInforeg], 1

.keyboardContinue:
    call keyboard_master


    endOfHardwareInterrupt
    popState
    iretq


_interrupt_timerTick:
	pushState

	call timer_master

	endOfHardwareInterrupt
	popState
	iretq


_exception_divideByZero:
	saveRegistersException
	
	mov rdi, 00h
	mov rsi, regdata_exc
	call exception_master
	
	mov rax, QWORD codeModule
	mov [rsp], rax 
	call getStackBase
	mov [rsp+24], rax
	iretq

_exception_invalidOpCode:
	saveRegistersException

	mov rdi, 06h
	mov rsi, regdata_exc
	call exception_master

	mov rax, QWORD codeModule
	mov [rsp], rax 
	call getStackBase
	mov [rsp+24], rax
	iretq



; en  asm: RDI RSI	RDX	R10	R8 R9
; en    C: RDI RSI RDX RCX R8 R9
_interrupt_syscall:
	;mov rcx, r10                        ESTO BUGEABA EL PASAJE DE PARAMETROS
	mov r9, rax
	call sys_master
	iretq

;;;;
_initialize_stack_frame:
    mov r8, rsp
    mov r9, rbp
    mov rsp, rdx
    mov rbp, rdx
    push 0x0           ; SS
    push rdx           ; RSP
    push 0x202         ; RFLAGS
    push 0x8           ; CS
    push rdi           ; <--- Dirección de la función principal
    mov rdi, rsi       ; Primer argumento
    mov rsi, rcx       ; Segundo argumento (si lo necesitás)
    pushState
    mov rax, rsp
    mov rsp, r8
    mov rbp, r9
    ret


_irq00handler:
	pushState
	
	mov rdi, 0
	call irqDispatcher  ;manejo de interrupciones llama a timer_master
	
	mov rdi, rsp
	call schedule
	mov rsp, rax
	mov al, 20h
	out 20h, al
	popState
	iretq
;;;;

;Keyboard
_irq01handler:
	irqHandlerMaster 1

;Cascade pic never called
_irq02handler:
	irqHandlerMaster 2

;Serial Port 2 and 4
_irq03handler:
	irqHandlerMaster 3

;Serial Port 1 and 3
_irq04handler:
	irqHandlerMaster 4

;USB
_irq05handler:
	irqHandlerMaster 5

haltcpu:
	cli
	hlt
	ret

forceTimerTick:
  int 0x20
  ret





SECTION .bss
	aux resq 1
	regdata_exc		resq	18	
	inforeg	resq	17	
	hasInforeg 		resb 	1 	
	left_shift  	resb 	1   
	ctrl_pressed resb 1
	codeModule equ 0x400000 

