//Feel free to use it in your port too, but please keep authorship!
//Touch Controls made by: VDavid003
#ifdef TOUCH_CONTROLS
#include <ultra64.h>
#include <PR/ultratypes.h>
#include <PR/gbi.h>

#include "config.h"
#include "sm64.h"
#include "game/game_init.h"
#include "game/memory.h"
#include "game/segment2.h"
#include "game/time_trials.h"
#include "game/level_update.h"
#include "gfx_dimensions.h"
#include "pc/gfx/gfx_pc.h"
#include "pc/configfile.h"

#include "controller_api.h"
#include "controller_touchscreen.h"

#define SCREEN_WIDTH_API 1280
#define SCREEN_HEIGHT_API 960

#define LEFT_EDGE ((int)floorf(SCREEN_WIDTH_API / 2 - SCREEN_HEIGHT_API / 2 * gfx_current_dimensions.aspect_ratio))
#define RIGHT_EDGE ((int)ceilf(SCREEN_WIDTH_API / 2 + SCREEN_HEIGHT_API / 2 * gfx_current_dimensions.aspect_ratio))

#define CORRECT_TOUCH_X(x) ((x * (RIGHT_EDGE - LEFT_EDGE)) + LEFT_EDGE)
#define CORRECT_TOUCH_Y(y) (y * SCREEN_HEIGHT_API)

#define JOYSTICK_SIZE 420

s16 before_x;
s16 before_y;
s16 touch_x;
s16 touch_y;
s16 touch_cam_last_x;
s16 touch_cam_last_y;
u8 swap_start;
static u32 timer = 0;

enum ControlElementType {
    Joystick,
    Button
};

struct Position {
    s32 x,y;
};

struct ControlElement {
    enum ControlElementType type;
    struct Position (*GetPos)();
    u8 touchID; //0 = not being touched, 1-255 = Finger being used
    //Joystick
    int joyX, joyY;
    //Button
    int buttonID;
    char character;
    int slideTouch;
};

#include "controller_touchscreen_layouts.inc"

static struct ControlElement *ControlElements = ControlElementsDefault;
static int ControlElementsLength = sizeof(ControlElementsDefault)/sizeof(struct ControlElement);

#define TRIGGER_DETECT(size) (((pos.x + size / 2 > CORRECT_TOUCH_X(event->x)) && (pos.x - size / 2 < CORRECT_TOUCH_X(event->x))) &&\
                              ((pos.y + size / 2 > CORRECT_TOUCH_Y(event->y)) && (pos.y - size / 2 < CORRECT_TOUCH_Y(event->y))))

void touch_down(struct TouchEvent* event) {
    struct Position pos;
    for(int i = 0; i < ControlElementsLength; i++) {
        if (ControlElements[i].touchID == 0) {
            pos = ControlElements[i].GetPos();
            switch (ControlElements[i].type) {
                case Joystick:
                    if (TRIGGER_DETECT(JOYSTICK_SIZE)) {
                        ControlElements[i].touchID = event->touchID;
                        ControlElements[i].joyX = CORRECT_TOUCH_X(event->x) - pos.x;
                        ControlElements[i].joyY = CORRECT_TOUCH_Y(event->y) - pos.y;
                    }
                    break;
                case Button:
                    if (TRIGGER_DETECT(240)) {
                        ControlElements[i].touchID = event->touchID;
                    }
                    break;
            }
        }
    }
}

void touch_motion(struct TouchEvent* event) {
    struct Position pos;
    if (timer != gGlobalTimer && CORRECT_TOUCH_X(event->x) > SCREEN_WIDTH_API / 2 && CORRECT_TOUCH_Y(event->y) < SCREEN_HEIGHT_API * 7 / 10) {
        if (before_x > 0)
            touch_x = CORRECT_TOUCH_X(event->x) - before_x;
        before_x = CORRECT_TOUCH_X(event->x);
        if (before_y > 0)
            touch_y = CORRECT_TOUCH_Y(event->y) - before_y;
        if (touch_x < configStickDeadzone / 4)
            touch_x = 0;
        if (touch_y < configStickDeadzone / 4)
            touch_y = 0;
        before_y = CORRECT_TOUCH_Y(event->y);
        timer = gGlobalTimer;
    }
    for(int i = 0; i < ControlElementsLength; i++) {
        pos = ControlElements[i].GetPos();
        if (ControlElements[i].touchID == event->touchID) {
            pos = ControlElements[i].GetPos();
                switch (ControlElements[i].type) {
                    case Joystick:
                        ; //workaround
                        s32 x,y;
                        x = CORRECT_TOUCH_X(event->x) - pos.x;
                        y = CORRECT_TOUCH_Y(event->y) - pos.y;
                        if (pos.x + JOYSTICK_SIZE/2 < CORRECT_TOUCH_X(event->x))
                            x = JOYSTICK_SIZE/2;
                        if (pos.x - JOYSTICK_SIZE/2 > CORRECT_TOUCH_X(event->x))
                            x = -JOYSTICK_SIZE/2;
                        if (pos.y + JOYSTICK_SIZE/2 < CORRECT_TOUCH_Y(event->y))
                            y = JOYSTICK_SIZE/2;
                        if (pos.y - JOYSTICK_SIZE/2 > CORRECT_TOUCH_Y(event->y))
                            y = -JOYSTICK_SIZE/2;

                        ControlElements[i].joyX = x;
                        ControlElements[i].joyY = y;
                        break;
                    case Button:
                        if (ControlElements[i].slideTouch && !TRIGGER_DETECT(240)) {
                            ControlElements[i].slideTouch = 0;
                            ControlElements[i].touchID = 0;
                        }
                        break;
                }
        }
        else {
            switch (ControlElements[i].type) {
                case Joystick:
                    break;
                case Button:
                    if (TRIGGER_DETECT(240)) {
                        if (ControlElements[i].character == 'b' || ControlElements[i].character == 'f')
                            ControlElements[i].slideTouch = 1;
                        ControlElements[i].touchID = event->touchID;
                    }
                    break;
            }
        }
    }
}

static void handle_touch_up(int i) {//seperated for when the layout changes
    ControlElements[i].touchID = 0;
    switch (ControlElements[i].type) {
        case Joystick:
            ControlElements[i].joyX = 0;
            ControlElements[i].joyY = 0;
            break;
        case Button:
            break;
    }
}

void touch_up(struct TouchEvent* event) {
    if (gGlobalTimer - timer > 1 || (CORRECT_TOUCH_X(event->x) > SCREEN_WIDTH_API / 2 && CORRECT_TOUCH_Y(event->y) < SCREEN_HEIGHT_API * 7 / 10)) {
        touch_x = before_x = 0;
        touch_y = before_y = 0;
    }
    for(int i = 0; i < ControlElementsLength; i++) {
        if (ControlElements[i].touchID == event->touchID) {
            handle_touch_up(i);
        }
    }
}

ALIGNED8 static const u8 texture_button[] = {
#include "textures/touchcontrols/touch_button.rgba16.inc.c"
};

ALIGNED8 static const u8 texture_button_dark[] = {
#include "textures/touchcontrols/touch_button_dark.rgba16.inc.c"
};

ALIGNED8 static const u8 texture_zeta[] = {
#include "textures/segment2/segment2.04600.rgba16.inc.c"
};

ALIGNED8 static const u8 texture_up[] = {
#include "textures/touchcontrols/segment2.08150.rgba16.inc.c"
};

ALIGNED8 static const u8 texture_down[] = {
#include "textures/touchcontrols/segment2.081D0.rgba16.inc.c"
};

ALIGNED8 static const u8 texture_left[] = {
#include "textures/touchcontrols/segment2.08B50.rgba16.inc.c"
};

ALIGNED8 static const u8 texture_right[] = {
#include "textures/touchcontrols/segment2.08BD0.rgba16.inc.c"
};

//Sprite drawing code stolen from src/game/print.c

static void select_button_texture(int dark) {

    gDPPipeSync(gDisplayListHead++);
    if (!dark) {
        gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_button);
    }
    else {
        gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_button_dark);
    }
    gSPDisplayList(gDisplayListHead++, dl_hud_img_load_tex_block);
}

static void select_char_texture(char num) {
    const u8 *const *glyphs = segmented_to_virtual(main_hud_lut);

    gDPPipeSync(gDisplayListHead++);
    gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, glyphs[num - 87]);
    gSPDisplayList(gDisplayListHead++, dl_hud_img_load_tex_block);
}

static void DrawSprite(s32 x, s32 y, int scaling) {
    gSPTextureRectangle(gDisplayListHead++, x - (15 << scaling), y - (15 << scaling), x + (15 << scaling),
                        y + (15 << scaling), G_TX_RENDERTILE, 0, 0, 4 << (11 - scaling), 1 << (11 - scaling));
}

void render_touch_controls(void) {
    swap_start = ControlElements[0].joyY == 0 && ControlElements[0].joyX == 0 ? 0 : 1;
	if(configInputDisplay && !(sTimeTrialsCam[0] != 0 || sTimeTrialsCam[1] != 0 || sTimeTrialsCam[2] != 0)){
    Mtx *mtx;

    mtx = alloc_display_list(sizeof(*mtx));

    if (mtx == NULL) {
        return;
    }

    guOrtho(mtx, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);
    gSPPerspNormalize((Gfx *) (gDisplayListHead++), 0xFFFF);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(mtx), G_MTX_PROJECTION | G_MTX_LOAD | G_MTX_NOPUSH);
    gSPDisplayList(gDisplayListHead++, dl_hud_img_begin);

    struct Position pos;
    for (int i = 0; i < ControlElementsLength; i++) {
        select_button_texture(0);
        switch (ControlElements[i].type) {
            case Joystick:
                pos = ControlElements[i].GetPos();
                DrawSprite(pos.x, pos.y, 3);
                DrawSprite(pos.x + 4 + ControlElements[i].joyX, pos.y + 4 + ControlElements[i].joyY, 2);
                if (before_x > 0 || before_y > 0) {
                    touch_cam_last_x = before_x > 0 ? before_x : touch_cam_last_x;
                    touch_cam_last_y = before_y > 0 ? before_y : touch_cam_last_y;
                    DrawSprite(touch_cam_last_x, touch_cam_last_y, 2);
                }
                break;
            case Button:
                if (ControlElements[i].touchID)
                    select_button_texture(1);
                pos = ControlElements[i].GetPos();
                DrawSprite(pos.x - 8, pos.y, 2);
				if (ControlElements[i].character == 'z') {
					gDPPipeSync(gDisplayListHead++);
					gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_zeta);
					gSPDisplayList(gDisplayListHead++, dl_hud_img_load_tex_block);
					DrawSprite(pos.x, pos.y, 1);
				} else if (ControlElements[i].character == 'u') {
					gDPPipeSync(gDisplayListHead++);
					gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_up);
					gSPDisplayList(gDisplayListHead++, dl_hud_img_load_tex_block);
					DrawSprite(pos.x - 5, pos.y - 10, 1);
				} else if (ControlElements[i].character == 'd') {
					gDPPipeSync(gDisplayListHead++);
					gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_down);
					gSPDisplayList(gDisplayListHead++, dl_hud_img_load_tex_block);
					DrawSprite(pos.x - 5, pos.y + 15, 1);
				} else if (ControlElements[i].character == 'l') {
					gDPPipeSync(gDisplayListHead++);
					gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_left);
					gSPDisplayList(gDisplayListHead++, dl_hud_img_load_tex_block);
					DrawSprite(pos.x - 18, pos.y + 2, 1);
				} else if (ControlElements[i].character == 'e') {
					gDPPipeSync(gDisplayListHead++);
					gDPSetTextureImage(gDisplayListHead++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, texture_right);
					gSPDisplayList(gDisplayListHead++, dl_hud_img_load_tex_block);
					DrawSprite(pos.x + 8, pos.y + 2, 1);
				} else {
					select_char_texture(ControlElements[i].character);
					DrawSprite(pos.x, pos.y, 1);
				}
                break;
        }
    }

    gSPDisplayList(gDisplayListHead++, dl_hud_img_end);
    }
}

static void touchscreen_init(void) {
}

static void touchscreen_read(OSContPad *pad) {
    for(int i = 0; i < ControlElementsLength; i++) {
        switch (ControlElements[i].type) {
            case Joystick:
                if (ControlElements[i].joyX || ControlElements[i].joyY) {
                    pad->stick_x = (ControlElements[i].joyX + JOYSTICK_SIZE/2) * 255 / JOYSTICK_SIZE - 128;
                    pad->stick_y = (-ControlElements[i].joyY + JOYSTICK_SIZE/2) * 255 / JOYSTICK_SIZE - 128; //inverted for some reason
                }
                break;
            case Button:
                if (ControlElements[i].touchID) {
                    pad->button |= ControlElements[i].buttonID;
                }
                break;
        }
    }
}

static u32 touchscreen_rawkey(void) { //dunno what this does but I'll skip it for now
    return VK_INVALID;
}

struct ControllerAPI controller_touchscreen = {
    0,
    touchscreen_init,
    touchscreen_read,
    touchscreen_rawkey,
    NULL,
    NULL,
    NULL,
    NULL
};
#endif
