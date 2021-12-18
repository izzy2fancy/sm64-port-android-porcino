#ifndef CONTROLLER_KEYBOARD_H
#define CONTROLLER_KEYBOARD_H

#include <stdbool.h>
#include "controller_api.h"

# define VK_BASE_KEYBOARD 0x0000

#ifdef __cplusplus
extern "C" {
#endif
bool keyboard_on_key_down(int scancode);
bool keyboard_on_key_up(int scancode);
void keyboard_on_all_keys_up(void);
#ifdef __cplusplus
}
#endif

extern int pressed_esc;
extern int pressed_r;
extern int pressed_enter;
extern bool pressed_left;
extern bool pressed_down;
extern bool pressed_right;
extern bool pressed_up;
extern bool pressed_del;
#ifdef BETTERCAMERA
extern bool mouseCursor;
#endif
extern struct ControllerAPI controller_keyboard;

#endif
