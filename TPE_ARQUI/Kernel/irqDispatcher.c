#include <time.h>
#include <stdint.h>
#include <videoDriver.h>
#include <keyboard.h>

#define SYS_CALLS_QTY 3

extern uint8_t hasInforeg;
extern const uint64_t inforeg[17];
extern uint64_t getSeconds();
extern uint64_t getMinutes();
extern uint64_t getHours();
extern int _hlt();

static Color WHITE = {255, 255, 255};
static void int_20();

void irqDispatcher(uint64_t irq)
{
	switch (irq)
	{
	case 0:
		int_20();
		break;
	}
	return;
}

void int_20()
{
	timer_master();
}

static uint64_t sys_read(uint64_t fd, char *buff)
{
	if (fd != 0)
	{
		return -1;
	}

	*buff = getCharFromKeyboard();

	return 0;
}

static uint64_t sys_write(uint64_t fd, char buffer)
{
	if (fd != 1)
	{
		return -1;
	}

	driver_printChar(buffer, WHITE);
	return 1;
}

static uint64_t sys_write_color(uint64_t fd, char buffer, Color fnt)
{
	if (fd != 1)
	{
		return -1;
	}

	driver_printChar(buffer, fnt);
	return 1;
}

static uint64_t sys_clear()
{
	driver_clear();
	return 1;
}

static uint64_t sys_getSeconds()
{
	uint64_t seconds = getSeconds();
    
    // Convert from BCD to binary if necessary
    return ((seconds / 16) * 10) + (seconds % 16);
}

static uint64_t sys_getMinutes()
{
	uint64_t minutes = getMinutes();
		
		// Convert from BCD to binary if necessary
	return ((minutes / 16) * 10) + (minutes % 16);
	
}

static uint64_t sys_getHours()
{
	uint64_t hours = getHours();
	
	// Convert from BCD to binary if necessary
	return ((hours / 16) * 10) + (hours % 16);
}

static uint64_t sys_increment_size()
{
	driver_increment_size();
	return 1;
}

static uint64_t sys_decrement_size()
{
	driver_decrement_size();
	return 1;
}

static uint64_t sys_drawSquare(int x, int y, uint32_t fillColor){
	drawSquare(x, y, fillColor);
	return 1;
}

static uint64_t sys_inforeg(uint64_t registers[17])
{
	if (hasInforeg)
	{
		for (uint8_t i = 0; i < 17; i++)
		{
			registers[i] = inforeg[i];
		}
	}
	return hasInforeg;
}

static uint64_t sys_sleep(uint64_t ms)
{
	if (ms > 0)
    {
        int start_ms = ms_elapsed();
        do
        {
            _hlt();
        } while (ms_elapsed() - start_ms < ms);
	}
	return 1;
}

static uint64_t (*sys_masters[])(uint64_t, uint64_t, uint64_t, uint64_t, uint64_t) = {
		(void *)sys_read,						// 0
		(void *)sys_write,					// 1
		(void *)sys_write_color,		// 2
		(void *)sys_clear,					// 3
		(void *)sys_increment_size, // 4
		(void *)sys_decrement_size, // 5
		(void *)sys_getHours,				// 6
		(void *)sys_getMinutes,			// 7
		(void *)sys_getSeconds,			// 8
		(void *)sys_inforeg,				// 9
		(void *)sys_drawSquare,			// 10
		(void *)sys_sleep						// 11
};

// Devuelve la syscall correspondiente
//                           rdi           rsi           rdx           rd10          r8           r9
uint64_t sys_master(uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t r10, uint64_t r8, uint64_t rax)
{
	if (rax < sizeof(sys_masters) / sizeof(sys_masters[0]) && sys_masters[rax] != 0)
	{
		return sys_masters[rax](rdi, rsi, rdx, r10, r8);
	}

	return 0;
}
