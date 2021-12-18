#include "sm64.h"
#include "geo_commands.h"
#include "gfx_dimensions.h"
#include "game/game_init.h"
#include "game/level_update.h"
#include "game/object_list_processor.h"
#include "pc/controller/controller_recorded_tas.h"
#include "pc/configfile.h"
#ifdef BETTERCAMERA
#include "pc/controller/controller_mouse.h"
extern u8 newcam_mouse;
extern u8 newcam_active;
#endif

//
// Data
//

static struct GraphNode *controller_display_graph_node = NULL;
static u8 controller_display_geo_pool[0x100];
static Gfx controller_display_gfx_pool[0x1000];
static Mtx controller_display_mtx_pool[0x10];
static Gfx *controller_display_gfx = controller_display_gfx_pool;
static Mtx *controller_display_mtx = controller_display_mtx_pool;

static void controller_display_init_gfx() {
    controller_display_gfx = controller_display_gfx_pool;
}

static void controller_display_init_mtx() {
    controller_display_mtx++;
    if (controller_display_mtx >= controller_display_mtx_pool + 0x10) {
        controller_display_mtx = controller_display_mtx_pool;
    }
}

//
// Glyphs
//

enum {
    GLYPH_ANALOG_STICK = 24,
    GLYPH_BUTTON_A = 10,
    GLYPH_BUTTON_B = 11,
    GLYPH_BUTTON_X = 50,
    GLYPH_BUTTON_Y = 34,
    GLYPH_BUTTON_L = 21,
    GLYPH_BUTTON_R = 27,
    GLYPH_BUTTON_Z = 35,
    GLYPH_BUTTON_START = 28,
    GLYPH_BUTTON_C = 12,
    GLYPH_BUTTON_D = 13,
    GLYPH_BUTTON_UP = 300,
    GLYPH_BUTTON_DOWN = 301,
    GLYPH_BUTTON_LEFT = 302,
    GLYPH_BUTTON_RIGHT = 303,
};

#define controller_display_button_up_texture_rgba16 main_hud_camera_lut[4]
#define controller_display_button_down_texture_rgba16 main_hud_camera_lut[5]
#define controller_display_button_left_texture_rgba16 main_hud_camera_lut[6]
#define controller_display_button_right_texture_rgba16 main_hud_camera_lut[7]

static void controller_display_render_glyph(s32 glyph, s32 x, s32 y, s32 opaque) {
    bool ghost = false;
    if (sTimeTrialsCam[0] != 0 || sTimeTrialsCam[1] != 0 || sTimeTrialsCam[2] != 0)
        ghost = true;
    u8 i = (opaque && (ghost) ? 255 : ((x == 20 || x == 66) && y == 36 && (ghost) ? 190 : 0));
    x = GFX_DIMENSIONS_FROM_LEFT_EDGE(x);
    y = SCREEN_HEIGHT - 18 - y;
    if (glyph > 255) {
        gDPPipeSync(controller_display_gfx++);
        gDPSetCombineLERP(controller_display_gfx++, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0, TEXEL0, 0, ENVIRONMENT, 0);
        gDPSetTextureFilter(controller_display_gfx++, G_TF_POINT);
        gDPPipeSync(controller_display_gfx++);
        gDPSetTexturePersp(controller_display_gfx++, G_TP_NONE);
        gDPSetBlendColor(controller_display_gfx++, i, i, i, i);
        gDPSetRenderMode(controller_display_gfx++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);
        gDPSetEnvColor(controller_display_gfx++, i, i, i, i);
        gDPSetTile(controller_display_gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD, G_TX_WRAP | G_TX_NOMIRROR, G_TX_NOMASK, G_TX_NOLOD);
        gDPTileSync(controller_display_gfx++);
        gDPSetTile(controller_display_gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 2, 0, G_TX_RENDERTILE, 0, G_TX_CLAMP, 3, G_TX_NOLOD, G_TX_CLAMP, 3, G_TX_NOLOD);
        gDPSetTileSize(controller_display_gfx++, G_TX_RENDERTILE, 0, 0, (8 - 1) << G_TEXTURE_IMAGE_FRAC, (8 - 1) << G_TEXTURE_IMAGE_FRAC);
        gDPPipeSync(controller_display_gfx++);
        switch (glyph) {
            case GLYPH_BUTTON_UP: gDPSetTextureImage(controller_display_gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, controller_display_button_up_texture_rgba16); break;
            case GLYPH_BUTTON_DOWN: gDPSetTextureImage(controller_display_gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, controller_display_button_down_texture_rgba16); break;
            case GLYPH_BUTTON_LEFT: gDPSetTextureImage(controller_display_gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, controller_display_button_left_texture_rgba16); break;
            case GLYPH_BUTTON_RIGHT: gDPSetTextureImage(controller_display_gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, controller_display_button_right_texture_rgba16); break;
        }
        gDPLoadSync(controller_display_gfx++);
        gDPLoadBlock(controller_display_gfx++, G_TX_LOADTILE, 0, 0, 8 * 8 - 1, CALC_DXT(8, G_IM_SIZ_16b_BYTES));
        gSPTextureRectangle(controller_display_gfx++, x << 2, y << 2, (x + 8) << 2, (y + 8) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        gSPDisplayList(controller_display_gfx++, dl_hud_img_end);
    } else {
        gSPDisplayList(controller_display_gfx++, dl_rgba16_text_begin);
        gDPPipeSync(controller_display_gfx++);
        gDPSetEnvColor(controller_display_gfx++, i, i, i, i);
        gDPSetTextureImage(controller_display_gfx++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 1, ((const u8 **) main_hud_lut)[glyph]);
        gSPDisplayList(controller_display_gfx++, dl_rgba16_load_tex_block);
        gSPTextureRectangle(controller_display_gfx++, x << 2, y << 2, (x + 16) << 2, (y + 16) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
        gSPDisplayList(controller_display_gfx++, dl_rgba16_text_end);
    }
}

static Gfx *geo_controller_display_update(s32 callContext, UNUSED struct GraphNode *node, UNUSED void *context) {
    bool ghost = false;
    if (sTimeTrialsCam[0] != 0 || sTimeTrialsCam[1] != 0 || sTimeTrialsCam[2] != 0)
        ghost = true;
    const struct Controller *cont = gMarioState->controller;
    if (cont && callContext == GEO_CONTEXT_RENDER && configHUD && gCamera->cutscene != CUTSCENE_ENDING && gCamera->cutscene != CUTSCENE_CREDITS && gCamera->cutscene != CUTSCENE_INTRO_PEACH && gCamera->cutscene != CUTSCENE_END_WAVING) {
        controller_display_init_gfx();
        controller_display_init_mtx();
        guOrtho(controller_display_mtx, 0.0f, SCREEN_WIDTH, 0.0f, SCREEN_HEIGHT, -10.0f, 10.0f, 1.0f);
        gSPPerspNormalize(controller_display_gfx++, 0xFFFF);
        gSPMatrix(controller_display_gfx++, VIRTUAL_TO_PHYSICAL(controller_display_mtx), G_MTX_MODELVIEW | G_MTX_NOPUSH);

        // Buttons
        controller_display_render_glyph(GLYPH_BUTTON_A, 4, 4, ghost ? sTimeTrialsButtons[0] : cont->buttonDown & A_BUTTON);
        controller_display_render_glyph(GLYPH_BUTTON_B, 20, 4, ghost ? sTimeTrialsButtons[1] : cont->buttonDown & B_BUTTON);
        controller_display_render_glyph(GLYPH_BUTTON_X, 100, 4, ghost ? sTimeTrialsButtons[2] : cont->buttonDown & 0x0040);
        controller_display_render_glyph(GLYPH_BUTTON_Y, 116, 4, ghost ? sTimeTrialsButtons[3] : cont->buttonDown & 0x0080);
        controller_display_render_glyph(GLYPH_BUTTON_L, 52, 4, ghost ? sTimeTrialsButtons[4] : cont->buttonDown & L_TRIG);
        controller_display_render_glyph(GLYPH_BUTTON_R, 68, 4, ghost ? sTimeTrialsButtons[5] : cont->buttonDown & R_TRIG);
        controller_display_render_glyph(GLYPH_BUTTON_Z, 36, 4, ghost ? sTimeTrialsButtons[6] : cont->buttonDown & Z_TRIG);
        controller_display_render_glyph(GLYPH_BUTTON_START, 84, 4, cont->buttonDown & START_BUTTON);

        // Analog stick
        controller_display_render_glyph(GLYPH_ANALOG_STICK, 20, 36, 0);
        controller_display_render_glyph(GLYPH_ANALOG_STICK, 20 + (((ghost ? sTimeTrialsStick[0] : cont->stickX) / 64.f) * 16.f), 36 + (((ghost ? sTimeTrialsStick[1] : cont->stickY) / 64.f) * 16.f), ((ghost ? sTimeTrialsStick[0] : cont->stickX) != 0 || (ghost ? sTimeTrialsStick[1] : cont->stickY) != 0));
		
#ifdef BETTERCAMERA
        // Mouse
        controller_display_render_glyph(GLYPH_BUTTON_UP, 65 + (newcam_mouse && newcam_active ? 1 : 0), 36, 0);
        controller_display_render_glyph(GLYPH_BUTTON_UP, 65 + (newcam_mouse && newcam_active ? 1 : 0) + (((s16)(ghost ? sTimeTrialsMouse[0] : mouse_x) / 64.f) * 4.f), 36 + (((s16)(ghost ? sTimeTrialsMouse[1] : mouse_y) / 64.f) * (-4.f)), newcam_mouse && newcam_active ? ((s16)(ghost ? sTimeTrialsMouse[0] : mouse_x) != 0 || (s16)(ghost ? sTimeTrialsMouse[1] : mouse_y) != 0) : 0);
#else
        u8 newcam_mouse = 0;
        u8 newcam_active = 0;
#endif

        // C-buttons
        controller_display_render_glyph(GLYPH_BUTTON_C, 63, 36, (cont->buttonDown & (U_CBUTTONS | D_CBUTTONS | L_CBUTTONS | R_CBUTTONS)) && (!newcam_active || !newcam_mouse));
        controller_display_render_glyph(GLYPH_BUTTON_UP, 67, 46, ghost ? sTimeTrialsButtons[7] : cont->buttonDown & U_CBUTTONS);
        controller_display_render_glyph(GLYPH_BUTTON_DOWN, 67, 18, ghost ? sTimeTrialsButtons[8] : cont->buttonDown & D_CBUTTONS);
        controller_display_render_glyph(GLYPH_BUTTON_LEFT, 52, 32, ghost ? sTimeTrialsButtons[9] : cont->buttonDown & L_CBUTTONS);
        controller_display_render_glyph(GLYPH_BUTTON_RIGHT, 81, 32, ghost ? sTimeTrialsButtons[10] : cont->buttonDown & R_CBUTTONS);

        // D-pad
        controller_display_render_glyph(GLYPH_BUTTON_D, 104, 36, cont->buttonDown & (U_JPAD | D_JPAD | L_JPAD | R_JPAD));
        controller_display_render_glyph(GLYPH_BUTTON_UP, 107, 46, cont->buttonDown & U_JPAD);
        controller_display_render_glyph(GLYPH_BUTTON_DOWN, 107, 18, cont->buttonDown & D_JPAD);
        controller_display_render_glyph(GLYPH_BUTTON_LEFT, 92, 32, cont->buttonDown & L_JPAD);
        controller_display_render_glyph(GLYPH_BUTTON_RIGHT, 121, 32, cont->buttonDown & R_JPAD);

        // Link to the master display list
        gDPSetEnvColor(controller_display_gfx++, 255, 255, 255, 255);
        gSPEndDisplayList(controller_display_gfx++);
        gSPDisplayList(gDisplayListHead + 0, gDisplayListHead + 3);
        gSPDisplayList(gDisplayListHead + 1, controller_display_gfx_pool);
        gSPEndDisplayList(gDisplayListHead + 2);
        gDisplayListHead += 3;
    }
    return NULL;
}

static const GeoLayout controller_display_geo[] = {
    GEO_NODE_START(),
    GEO_OPEN_NODE(),
    GEO_ASM(0, geo_controller_display_update),
    GEO_CLOSE_NODE(),
    GEO_END(),
};

static void bhv_controller_display_update() {
    if (gCamera) {
        gCurrentObject->oPosX = gCamera->focus[0];
        gCurrentObject->oPosY = gCamera->focus[1];
        gCurrentObject->oPosZ = gCamera->focus[2];
    }
}

static const BehaviorScript bhvControllerDisplay[] = {
    0x11010001,
    0x08000000,
    0x0C000000, (uintptr_t) bhv_controller_display_update,
    0x09000000,
};

static void (*controller_read)(OSContPad *pad);
static void controller_display_update(OSContPad *pad) {
  if (configInputDisplay) {
    controller_read(pad);
    if (gObjectLists && gMarioObject) {
        struct Object *head = (struct Object *) &gObjectLists[get_object_list_from_behavior(bhvControllerDisplay)];
        struct Object *next = (struct Object *) head->header.next;
        while (next != head) {
            if (next->behavior == bhvControllerDisplay) {
                return;
            }
            next = (struct Object *) next->header.next;
        }

        if (!controller_display_graph_node) {
            struct AllocOnlyPool *pool = (struct AllocOnlyPool *) controller_display_geo_pool;
            pool->totalSpace = sizeof(controller_display_geo_pool) - sizeof(struct AllocOnlyPool);
            pool->usedSpace = 0;
            pool->startPtr = (u8 *) pool + sizeof(struct AllocOnlyPool);
            pool->freePtr = (u8 *) pool + sizeof(struct AllocOnlyPool);
            controller_display_graph_node = process_geo_layout(pool, (void *) controller_display_geo);
        }
        struct Object *o = spawn_object(gMarioObject, 0, bhvControllerDisplay);
        o->header.gfx.sharedChild = controller_display_graph_node;
    }
  }
}

__attribute__((constructor))
static void controller_display_init() {
    controller_read = controller_recorded_tas.read;
    controller_recorded_tas.read = controller_display_update;
}
