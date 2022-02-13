#include "game/bettercamera.h"
#include "pc/controller/controller_mouse.h"
extern u8 newcam_mouse;

//Not gonna name it .inc.c because of makefile.
//This file will have more layouts for dynamic controls later.

//DEFAULT
struct Position GetDefaultJoystickPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(90) << 2, .y = 710 };
    return ret;
}

struct Position GetDefaultStartPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(15) << 2 , .y = swap_start ? -900 : 900 };
    return ret;
}

struct Position GetDefaultAPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(70) << 2, .y = 820 };
    return ret;
}

struct Position GetDefaultZPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_LEFT_EDGE(40) << 2, .y = 94 };
    return ret;
}

struct Position GetDefaultBPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(130) << 2, .y = 820 };
    return ret;
}

struct Position GetDefaultRPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(15) << 2, .y = 900 };
    return ret;
}

struct Position GetDefaultUpPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(15) << 2, .y = newcam_active && newcam_mouse ? -460 : 440 };
    return ret;
}

struct Position GetDefaultDownPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(15) << 2, .y = newcam_active && newcam_mouse ? -740 : 720 };
    return ret;
}

struct Position GetDefaultLeftPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(103) << 2, .y = newcam_active && newcam_mouse ? -600 : 580 };
    return ret;
}

struct Position GetDefaultRightPos() {
    struct Position ret = { .x = GFX_DIMENSIONS_RECT_FROM_RIGHT_EDGE(52) << 2, .y = newcam_active && newcam_mouse ? -600 : 580 };
    return ret;
}

static struct ControlElement ControlElementsDefault[] = {
{.type = Joystick, .GetPos = GetDefaultJoystickPos},
{.type = Button, .GetPos = GetDefaultStartPos, .character = 's', .buttonID = START_BUTTON},
{.type = Button, .GetPos = GetDefaultAPos, .character = 'a', .buttonID = A_BUTTON},
{.type = Button, .GetPos = GetDefaultBPos, .character = 'b', .buttonID = B_BUTTON},
{.type = Button, .GetPos = GetDefaultZPos, .character = 'z', .buttonID = Z_TRIG}, //There's no z character, so let's just call it t for trigger
{.type = Button, .GetPos = GetDefaultRPos, .character = 'r', .buttonID = R_TRIG},
{.type = Button, .GetPos = GetDefaultUpPos, .character = 'u', .buttonID = U_CBUTTONS},
{.type = Button, .GetPos = GetDefaultDownPos, .character = 'd', .buttonID = D_CBUTTONS},
{.type = Button, .GetPos = GetDefaultLeftPos, .character = 'l', .buttonID = L_CBUTTONS},
{.type = Button, .GetPos = GetDefaultRightPos, .character = 'e', .buttonID = R_CBUTTONS}
};
