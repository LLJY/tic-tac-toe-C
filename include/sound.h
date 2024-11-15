#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>

extern const char *BGM_SND;
extern const char *START_SND;
extern const char *BTN_CLICK_SND;
extern const char *WIN_SND;
extern const char *DRAW_SND;
extern const char *LOSE_SND;
extern const char *SURRENDER_SND;

/// @brief Initializes gstreamer and get it ready for sound playback
void init_audio();

// let the gui.h decide which file to play, we just provide the defines here
void play_sound(const char *sound_file, bool isRepeat);
#endif