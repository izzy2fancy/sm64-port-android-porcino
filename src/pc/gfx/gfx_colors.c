#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "game/area.h"
#include "game/level_update.h"
#include "game/game_init.h"
#include "game/object_list_processor.h"

static float *color;
static u32 timer = 0;

float *color_fx (void) {
	if (gGlobalTimer == timer && timer != 0) return color;
	color = malloc (sizeof (float) * 6);
	timer = gGlobalTimer;
	bool cam_underwater = false;
	color[0] = 1.f;
	color[1] = 1.f;
	color[2] = 1.f;
	color[3] = 1.f;
	color[4] = 1.f;
	color[5] = 1.f;
	if (timer == 0) return color;
	if (gNumStaticSurfaces > 10) {
		if ((gCurrLevelNum == 4 || gCurrLevelNum == 6 || gCurrLevelNum == 7 || gCurrLevelNum == 10 || gCurrLevelNum == 11 || gCurrLevelNum == 12 || gCurrLevelNum == 13 || gCurrLevelNum == 16 || gCurrLevelNum == 20 || gCurrLevelNum == 23 || gCurrLevelNum == 24 || gCurrLevelNum == 26 || gCurrLevelNum == 28 || gCurrLevelNum == 36) && gCurrentArea->camera && gCurrentArea->camera->cutscene != CUTSCENE_CREDITS){
			float cam_distance = sqrt(((gCurrentArea->camera->pos[0] - gCurrentArea->camera->focus[0]) * (gCurrentArea->camera->pos[0] - gCurrentArea->camera->focus[0])) + ((gCurrentArea->camera->pos[1] - gCurrentArea->camera->focus[1]) * (gCurrentArea->camera->pos[1] - gCurrentArea->camera->focus[1])) + ((gCurrentArea->camera->pos[2] - gCurrentArea->camera->focus[2]) * (gCurrentArea->camera->pos[2] - gCurrentArea->camera->focus[2])));
			double cam_angle = 0.0;
			if (cam_distance != 0)
				cam_angle = 1.f + asin((gCurrentArea->camera->pos[1] - gCurrentArea->camera->focus[1]) / cam_distance);
			if (cam_angle < 0.5) cam_angle = 0;
			cam_underwater = (gMarioState->waterLevel - gCurrentArea->camera->pos[1] - 30 + (30 * cam_angle)) > 0;
		}
		if (cam_underwater) {
			if (gCurrLevelNum == 12) {
				color[0] *= 0.5;
				color[1] *= 0.8;
				color[2] *= 0.92;
				color[3] *= 0.5;
				color[4] *= 0.8;
				color[5] *= 0.92;
			} else {
				color[0] *= 0.5;
				color[1] *= 0.7;
				color[2] *= 1.0;
				color[3] *= 0.5;
				color[4] *= 0.7;
				color[5] *= 1.0;
            }
		}
		//RED
		if (gCurrLevelNum == 19 || gCurrLevelNum == 22 || gCurrLevelNum == 33) {
			color[0] *= 1.0;
			color[1] *= 0.8;
			color[2] *= 0.7;
			color[3] *= 1.0;
			color[4] *= 0.8;
			color[5] *= 0.7;
		//CYAN
		} else if (gCurrLevelNum == 9 || gCurrLevelNum == 13 || gCurrLevelNum == 15 || gCurrLevelNum == 31) {
			color[0] *= 1.2;
			color[1] *= 1.3;
			color[2] *= 1.3;
			color[3] *= 0.9;
			color[4] *= 1.0;
			color[5] *= 1.0;
		//DARK
		} else if (gCurrLevelNum == 4 || gCurrLevelNum == 7 || gCurrLevelNum == 17 || gCurrLevelNum == 18 || gCurrLevelNum == 28 || gCurrLevelNum == 30) {
			color[0] *= .65;
			color[1] *= .65;
			color[2] *= .65;
			color[3] *= .65;
			color[4] *= .65;
			color[5] *= .65;
		//GREY
		} else if (gCurrLevelNum == 23 || gCurrLevelNum == 27) {
			color[0] *= .85;
			color[1] *= .85;
			color[2] *= .85;
			color[3] *= .85;
			color[4] *= .85;
			color[5] *= .85;
		//YELLOW
		} else if (gCurrLevelNum == 8 && gCurrAreaIndex == 1) {
			color[0] *= 1.3;
			color[1] *= 1.21;
			color[2] *= 1.05;
			color[3] *= 1.0;
			color[4] *= 0.95;
			color[5] *= 0.85;
		//ORANGE
		} else if (gCurrLevelNum == 11) {
			color[0] *= 1.0;
			color[1] *= 0.9;
			color[2] *= 0.8;
			color[3] *= 1.0;
			color[4] *= 0.9;
			color[5] *= 0.8;
		//MAGENTA
		} else if (gCurrLevelNum == 21 || gCurrLevelNum == 34) {
			color[0] *= 0.9;
			color[1] *= 0.8;
			color[2] *= 1.0;
			color[3] *= 0.9;
			color[4] *= 0.8;
			color[5] *= 1.0;
		//BLUE
		} else if (gCurrLevelNum == 12) {
			color[0] *= 0.8;
			color[1] *= 0.85;
			color[2] *= 1.0;
			color[3] *= 0.8;
			color[4] *= 0.85;
			color[5] *= 1.0;
		//GREEN
		} else if (gCurrLevelNum == 24 || gCurrLevelNum == 29) {
			color[0] *= 1.1;
			color[1] *= 1.15;
			color[2] *= 1.11;
			color[3] *= 0.95;
			color[4] *= 1.0;
			color[5] *= 0.96;
		}
	}
	return color;
}
