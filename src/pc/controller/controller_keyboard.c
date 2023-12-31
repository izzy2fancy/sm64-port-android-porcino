#include <stdbool.h>
#include <ultra64.h>
#include <stdlib.h>

#include "controller_api.h"

#ifdef TARGET_WEB
#include "controller_emscripten_keyboard.h"
#endif

#include "../configfile.h"
#include "controller_keyboard.h"
#ifdef BETTERCAMERA
#include "controller_mouse.h"
#include "game/bettercamera.h"
extern u8 newcam_mouse;
extern u8 newcam_mouse_to_stick;
bool mouseCursor;
#endif

static int keyboard_buttons_down;
int pressed_esc = 0;
int pressed_r = 0;
int pressed_enter = 0;
bool pressed_left = false;
bool pressed_down = false;
bool pressed_right = false;
bool pressed_up = false;
bool pressed_del = false;

#define MAX_KEYBINDS 64
static int keyboard_mapping[MAX_KEYBINDS][2];
static int num_keybinds = 0;

static u32 keyboard_lastkey = VK_INVALID;

static int keyboard_map_scancode(int scancode) {
    int ret = 0;
    for (int i = 0; i < num_keybinds; i++) {
        if (keyboard_mapping[i][0] == scancode) {
            ret |= keyboard_mapping[i][1];
        }
    }
    return ret;
}

bool keyboard_on_key_down(int scancode) {
    int mapped = keyboard_map_scancode(scancode);
    keyboard_buttons_down |= mapped;
    keyboard_lastkey = scancode;
    return mapped != 0;
}

bool keyboard_on_key_up(int scancode) {
    int mapped = keyboard_map_scancode(scancode);
    keyboard_buttons_down &= ~mapped;
    if (keyboard_lastkey == (u32) scancode)
        keyboard_lastkey = VK_INVALID;
    return mapped != 0;
}

void keyboard_on_all_keys_up(void) {
    keyboard_buttons_down = 0;
}

static void keyboard_add_binds(int mask, unsigned int *scancode) {
    for (int i = 0; i < MAX_BINDS && num_keybinds < MAX_KEYBINDS; ++i) {
        if (scancode[i] < VK_BASE_KEYBOARD + VK_SIZE) {
            keyboard_mapping[num_keybinds][0] = scancode[i];
            keyboard_mapping[num_keybinds][1] = mask;
            num_keybinds++;
        }
    }
}

static void keyboard_bindkeys(void) {
    bzero(keyboard_mapping, sizeof(keyboard_mapping));
    num_keybinds = 0;
    unsigned int key_esc[3]  = {0x0001, VK_INVALID, VK_INVALID};
    unsigned int key_r[3]    = {0x0013, VK_INVALID, VK_INVALID};
    unsigned int key_enter[3]= {0x001C, VK_INVALID, VK_INVALID};
    unsigned int key_left[3] = {0x014B, VK_INVALID, VK_INVALID};
    unsigned int key_down[3] = {0x0150, VK_INVALID, VK_INVALID};
    unsigned int key_right[3]= {0x014D, VK_INVALID, VK_INVALID};
    unsigned int key_up[3]   = {0x0148, VK_INVALID, VK_INVALID};
    unsigned int key_del[3]  = {0x0153, VK_INVALID, VK_INVALID};

    keyboard_add_binds(STICK_UP,     configKeyStickUp);
    keyboard_add_binds(STICK_LEFT,   configKeyStickLeft);
    keyboard_add_binds(STICK_DOWN,   configKeyStickDown);
    keyboard_add_binds(STICK_RIGHT,  configKeyStickRight);
    keyboard_add_binds(A_BUTTON,     configKeyA);
    keyboard_add_binds(B_BUTTON,     configKeyB);
    keyboard_add_binds(Z_TRIG,       configKeyZ);
    keyboard_add_binds(U_CBUTTONS,   configKeyCUp);
    keyboard_add_binds(L_CBUTTONS,   configKeyCLeft);
    keyboard_add_binds(D_CBUTTONS,   configKeyCDown);
    keyboard_add_binds(R_CBUTTONS,   configKeyCRight);
    keyboard_add_binds(L_TRIG,       configKeyL);
    keyboard_add_binds(R_TRIG,       configKeyR);
    keyboard_add_binds(START_BUTTON, configKeyStart);
    keyboard_add_binds(0x100000,     configKeyWalk);
    keyboard_add_binds(0x200000,     configKeyMouseToStick);
    keyboard_add_binds(0x400000,     key_esc);
	keyboard_add_binds(0x800000,     key_r);
	keyboard_add_binds(0x1000000,    key_enter);
	keyboard_add_binds(0x2000000,    key_left);
	keyboard_add_binds(0x4000000,    key_down);
	keyboard_add_binds(0x8000000,    key_right);
	keyboard_add_binds(0x10000000,   key_up);
	keyboard_add_binds(0x20000000,   key_del);
}

static void keyboard_init(void) {
    keyboard_bindkeys();

#ifdef TARGET_WEB
    controller_emscripten_keyboard_init();
#endif
}

static void keyboard_read(OSContPad *pad) {
    pad->button |= keyboard_buttons_down;
    const u32 xstick = keyboard_buttons_down & STICK_XMASK;
    const u32 ystick = keyboard_buttons_down & STICK_YMASK;
    const s8 sens = (keyboard_buttons_down & 0x100000) ? configSpeed * 10 / 4 + 30 : 127;
    if (xstick == STICK_LEFT)
        pad->stick_x = -sens;
    else if (xstick == STICK_RIGHT)
       pad->stick_x = sens;
    if (ystick == STICK_DOWN)
        pad->stick_y = -sens;
    else if (ystick == STICK_UP)
        pad->stick_y = sens;
    if (pad->stick_x != 0 && pad->stick_y != 0 && (keyboard_buttons_down & 0x100000)) {
        double angle = atan2(pad->stick_y, pad->stick_x);
        pad->stick_x = cos(angle) * abs(pad->stick_x);
        pad->stick_y = sin(angle) * abs(pad->stick_y);
    }
#ifdef BETTERCAMERA
    if (((keyboard_buttons_down & 0x200000) && newcam_mouse) || mouseCursor) {
        newcam_mouse_to_stick = 1;
        double turn_x = mouse_x > sens * 2.f ? sens * 2.f : mouse_x < -sens * 2.f ? -sens * 2.f : mouse_x;
        double turn_y = -mouse_y > sens * 2.f ? sens * 2.f : -mouse_y < -sens * 2.f ? -sens * 2.f : -mouse_y;
        turn_x = sens * sqrt((sqrt(turn_x * turn_x) > turn_x ? turn_x * (-1.f) : turn_x ) / sens) * (turn_x < 0 ? -1.f : 1.f) / 2.f;
        turn_y = sens * sqrt((sqrt(turn_y * turn_y) > turn_y ? turn_y * (-1.f) : turn_y ) / sens) * (turn_y < 0 ? -1.f : 1.f) / 2.f;
        pad->stick_x = pad->stick_x + (int)turn_x > sens ? sens : pad->stick_x + (int)turn_x < -sens ? -sens : pad->stick_x + (int)turn_x;
        pad->stick_y = pad->stick_y + (int)turn_y > sens ? sens : pad->stick_y + (int)turn_y < -sens ? -sens : pad->stick_y + (int)turn_y;
    } else {
        newcam_mouse_to_stick = 0;
    }
#endif
    if (keyboard_buttons_down & 0x400000)
        pressed_esc = pressed_esc == 2 ? 1 : 0;
    else
        pressed_esc = 2;
    if (keyboard_buttons_down & 0x800000)
        pressed_r = pressed_r == 2 ? 1 : 0;
    else
        pressed_r = 2;
    if (keyboard_buttons_down & 0x1000000)
        pressed_enter = pressed_enter == 2 ? 1 : 0;
    else
        pressed_enter = 2;
    pressed_left = keyboard_buttons_down & 0x2000000;
    pressed_down = keyboard_buttons_down & 0x4000000;
    pressed_right = keyboard_buttons_down & 0x8000000;
    pressed_up = keyboard_buttons_down & 0x10000000;
    pressed_del = keyboard_buttons_down & 0x20000000;
}

static u32 keyboard_rawkey(void) {
    const u32 ret = keyboard_lastkey;
    keyboard_lastkey = VK_INVALID;
    return ret;
}

static void keyboard_shutdown(void) {
}

struct ControllerAPI controller_keyboard = {
    VK_BASE_KEYBOARD,
    keyboard_init,
    keyboard_read,
    keyboard_rawkey,
    NULL,
    NULL,
    keyboard_bindkeys,
    keyboard_shutdown
};
