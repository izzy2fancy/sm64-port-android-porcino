#ifndef CONTROLLER_TOUCHSCREEN_H
#define CONTROLLER_TOUCHSCREEN_H
#ifdef TOUCH_CONTROLS

struct TouchEvent {
    int touchID; //Should not be 0
    float x, y; //Should be from 0 to 1
};

void touch_down(struct TouchEvent* event);
void touch_motion(struct TouchEvent* event);
void touch_up(struct TouchEvent* event);

extern struct ControllerAPI controller_touchscreen;
extern s16 touch_x;
extern s16 touch_y;
extern u8 swap_r;
extern u8 swap_start;

#endif
#endif
