#include <time.h>
#include <stdint.h>
#include <videoDriver.h>

#define SYS_CALLS_QTY 3

static Color WHITE = {255,255,255};
static void int_20();

void irqDispatcher(uint64_t irq) {
	switch (irq) {
		case 0:
			int_20();
			break;
	}
	return;
}

void int_20() {
	timer_handler();
}


static uint64_t sys_read(uint64_t fd, char * buff){
    if (fd != 0){
        return -1;
    }

    *buff = getCharFromKeyboard();

    return 0;
}

static uint64_t sys_write(uint64_t fd, char buffer) {
	if (fd != 1){
		return -1;
	}

	driver_printChar(buffer,WHITE);
	return 1;
}

static uint64_t sys_write_color(uint64_t fd, char buffer, Color fnt) {
	if (fd != 1){
		return -1;
	}

	driver_printChar(buffer,fnt);
	return 1;
}

static uint64_t sys_clear(){
	driver_clear();
	return 1;
}

static uint64_t sys_increment_size(){
	driver_increment_size();
	return 1;
}

static uint64_t sys_decrement_size(){
	driver_decrement_size();
	return 1;
}

static uint64_t (*syscall_handlers[])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = {
    (void*)sys_read, //0                                                                     
    (void*)sys_write, //1
	(void*)sys_write_color,//2
	(void*)sys_clear, //3
	(void*)sys_increment_size, //4
	(void*)sys_decrement_size //5
	//(void*)sys_getHours,//6
	//(void*)sys_getMinutes,//7
	//(void*)sys_getSeconds//8
};


// Devuelve la syscall correspondiente
//                           rdi           rsi           rdx           rd10          r8           r9
uint64_t syscall_handler(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax) {
    if (rax < sizeof(syscall_handlers) / sizeof(syscall_handlers[0]) && syscall_handlers[rax] != 0){
        return syscall_handlers[rax](rdi, rsi, rdx, r10, r8);
    }

    return 0;
}
