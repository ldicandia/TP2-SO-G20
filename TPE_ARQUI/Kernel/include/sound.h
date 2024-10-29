#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

// Function to stop the speaker
void stopSpeaker();

// Function to trigger the speaker with a specific frequency
void triggerSpeaker(uint32_t frequence);

// Function to beep with a specific frequency and duration
void beep(uint32_t freq, uint64_t duration);

// Mock functions for spkIn and spkOut (assuming they are defined elsewhere)
uint8_t spkIn(uint16_t port);
void spkOut(uint16_t port, uint8_t data);

#endif  // SOUND_H