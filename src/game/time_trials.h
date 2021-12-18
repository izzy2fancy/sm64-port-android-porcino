#ifndef TIME_TRIALS_H
#define TIME_TRIALS_H

#include <stdbool.h>
#include "types.h"

#define TIME_TRIALS time_trials_enabled()

extern Gfx *gTimeTableDisplayListHead;
extern bool sTimeTrialsEnabled;
extern s16 sTimeTrialsPos[3];
extern s16 sTimeTrialsCam[3];
extern s16 sTimeTrialsFocus[3];
extern bool sTimeTrialsButtons[11];
extern s16 sTimeTrialsStick[2];
extern s16 sTimeTrialsMouse[2];
extern s16 sTimeTrialsHealth;
bool time_trials_enabled();
void time_trials_update(bool isPaused);
void time_trials_save_time_and_stop_timer(u32 *noExit);
bool time_trials_render_time_table(s8 *index);
void time_trials_render_star_select_time(s32 starIndex);

#endif // TIME_TRIALS_H
