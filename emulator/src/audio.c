#include "include/audio.h"
#include <SDL2/SDL.h>
#include <math.h>
#include "include/ram.h"

SDL_AudioDeviceID audio_device;
SDL_AudioSpec audio_spec;

void tick_speaker(uint16_t frequency, uint8_t duration) {
	audio_spec.freq = frequency;

	if (audio_spec.freq == 0)
		return;

	float x = 0;
	for (int i = 0; i < ((duration >> 7) & 1 ? audio_spec.freq * (duration - (1 << 7) + 1) : (audio_spec.freq / duration)); i++) {
		x += .010f;

		int16_t sample = sin(x * 4) * 5000;

		const int sample_size = sizeof(int16_t) * 1;
		SDL_QueueAudio(audio_device, &sample, sample_size);
	}
}

void init_speaker() {
	DBG(1, printf("Initializing Audio");)

	SDL_zero(audio_spec);
	
	audio_spec.format = AUDIO_S16SYS;
	audio_spec.freq = 44100;
	audio_spec.channels = 1;
	audio_spec.samples = 2048;
	audio_spec.callback = NULL;

	audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
	SDL_PauseAudioDevice(audio_device, 0);

	DBG(1, printf("Initialized Audio");)
}

void destroy_speaker() {
	DBG(1, printf("Destroying Audio");)
	SDL_CloseAudioDevice(audio_device);
}