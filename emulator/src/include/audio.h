#ifndef AUDIO_H
#define AUDIO_H

#include "global.h"

void tick_speaker(uint16_t frequency, uint8_t duration);
void init_speaker();
void destroy_speaker();

#endif