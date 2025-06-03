// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <irqDispatcher.h>
#include <sound.h>
#include <stdint.h>
#include <time.h>

void stopSpeaker() {
	uint8_t tmp = spkIn(0x61) & 0xFC;
	spkOut(0x61, tmp);
}

void triggerSpeaker(uint32_t frequence) {
	uint32_t Div;
	uint8_t tmp;

	if (frequence == 0) {
		stopSpeaker();
		return;
	}

	Div = 1193180 / frequence;
	spkOut(0x43, 0xb6);
	spkOut(0x42, (uint8_t) (Div));
	spkOut(0x42, (uint8_t) (Div >> 8));

	tmp = spkIn(0x61);
	if (tmp != (tmp | 3)) {
		spkOut(0x61, tmp | 3);
	}
}

void beep(uint32_t freq, uint64_t duration) {
	triggerSpeaker(freq);
	sys_sleep(duration);
	stopSpeaker();
}