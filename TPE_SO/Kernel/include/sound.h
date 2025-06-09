#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

void stopSpeaker();

void triggerSpeaker(uint32_t frequence);

void beep(uint32_t freq, uint64_t duration);

uint8_t spkIn(uint16_t port);

void spkOut(uint16_t port, uint8_t data);

#endif // SOUND_H