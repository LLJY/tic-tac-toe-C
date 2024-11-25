#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>

extern const char *BGM_SND; // Background music file, played throughout the game.
extern const char *START_SND; // Start game music file, only played once
extern const char *BTN_CLICK_SND; // btn click music file
extern const char *WIN_SND; // win music file
extern const char *DRAW_SND; // Draw music file
extern const char *LOSE_SND; // Lose music file
extern const char *SURRENDER_SND; // Surrender music file

/// @brief Initializes gstreamer and get it ready for sound playback
void init_audio();

// let the gui.h decide which file to play, we just provide the defines here
void play_sound(const char *sound_file, bool isRepeat);
#endif