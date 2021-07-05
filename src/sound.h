#ifndef _SOUND_H_
#define _SOUND_H_

void audio_callback(void *data, unsigned char *buffer, int bytes);
void make_sound();
void init_audio();
void stop_audio();

#endif