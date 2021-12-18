#ifdef EXT_OPTIONS_MENU

#include "sm64.h"
#include "text_strings.h"
#include "engine/math_util.h"
#include "audio/external.h"
#include "game/camera.h"
#include "game/level_update.h"
#include "game/print.h"
#include "game/segment2.h"
#include "game/save_file.h"
#ifdef BETTERCAMERA
#include "game/bettercamera.h"
#endif
#include "game/mario_misc.h"
#include "game/game_init.h"
#include "game/ingame_menu.h"
#include "game/options_menu.h"
#include "pc/pc_main.h"
#include "pc/cliopts.h"
#include "pc/cheats.h"
#include "pc/configfile.h"
#include "pc/controller/controller_api.h"
#include "pc/controller/controller_keyboard.h"

#include <stdbool.h>
#include "../../include/libc/stdlib.h"

u8 optmenu_open = 0;

static u8 optmenu_binding = 0;
static u8 optmenu_bind_idx = 0;

/* Keeps track of how many times the user has pressed L while in the options menu, so cheats can be unlocked */
static s32 l_counter = 0;

// How to add stuff:
// strings: add them to include/text_strings.h.in
//          and to menuStr[] / opts*Str[]
// options: add them to the relevant options list
// menus:   add a new submenu definition and a new
//          option to the optsMain list

static const u8 toggleStr[][16] = {
    { TEXT_OPT_DISABLED },
    { TEXT_OPT_ENABLED },
};

static const u8 optSmallStr[][32] = {
    { TEXT_OPT_BUTTON1 },
    { TEXT_OPT_BUTTON2 },
    { TEXT_OPT_L_HIGHLIGHT },
    { TEXT_OPT_R_HIGHLIGHT },
};

static const u8 menuStr[][32] = {
    { TEXT_OPT_OPTIONS },
    { TEXT_OPT_CAMERA },
    { TEXT_OPT_CONTROLS },
    { TEXT_OPT_VIDEO },
    { TEXT_OPT_AUDIO },
    { TEXT_MAIN_MENU },
    { TEXT_EXIT_GAME },
    { TEXT_OPT_CHEATS },
};

static const u8 optsCameraStr[][32] = {
    { TEXT_OPT_CAMX },
    { TEXT_OPT_CAMY },
    { TEXT_OPT_INVERTX },
    { TEXT_OPT_INVERTY },
    { TEXT_OPT_CAMC },
    { TEXT_OPT_CAMP },
    { TEXT_OPT_ANALOGUE },
    { TEXT_OPT_MOUSE },
    { TEXT_OPT_CAMD },
    { TEXT_OPT_CAMON },
    { TEXT_OPT_CAMFOV },
};

static const u8 optsVideoStr[][32] = {
    { TEXT_OPT_FSCREEN },
    { TEXT_OPT_TEXFILTER },
    { TEXT_OPT_ANTIALIASING },
    { TEXT_OPT_NEAREST },
    { TEXT_OPT_LINEAR },
    { TEXT_OPT_RESETWND },
    { TEXT_OPT_VSYNC },
    { TEXT_OPT_AUTO },
    { TEXT_OPT_INPUTDISPLAY },
    { TEXT_OPT_FRAMESKIP },
    { TEXT_OPT_HUD },
    { TEXT_OPT_THREEPT },
    { TEXT_OPT_DRAWDIST },
    { TEXT_OPT_APPLY },
};

static const u8 optsAudioStr[][32] = {
    { TEXT_OPT_MVOLUME },    
    { TEXT_OPT_MUSVOLUME },
    { TEXT_OPT_SFXVOLUME },
    { TEXT_OPT_ENVVOLUME },
};

static const u8 optsCheatsStr[][64] = {
    { TEXT_OPT_CHEAT1 },
    { TEXT_OPT_CHEAT2 },
    { TEXT_OPT_CHEAT3 },
    { TEXT_OPT_CHEAT4 },
    { TEXT_OPT_CHEAT5 },
    { TEXT_OPT_CHEAT6 },
    { TEXT_OPT_CHEAT7 },
    { TEXT_OPT_CHEAT8 },
    { TEXT_OPT_CHEAT9 },
};

static const u8 bindStr[][32] = {
    { TEXT_OPT_UNBOUND },
    { TEXT_OPT_PRESSKEY },
    { TEXT_BIND_A },
    { TEXT_BIND_B },
    { TEXT_BIND_START },
    { TEXT_BIND_L },
    { TEXT_BIND_R },
    { TEXT_BIND_Z },
    { TEXT_BIND_C_UP },
    { TEXT_BIND_C_DOWN },
    { TEXT_BIND_C_LEFT },
    { TEXT_BIND_C_RIGHT },
    { TEXT_BIND_UP },
    { TEXT_BIND_DOWN },
    { TEXT_BIND_LEFT },
    { TEXT_BIND_RIGHT },
    { TEXT_BIND_MOUSE },
    { TEXT_BIND_WALK },
    { TEXT_BIND_SPEED },
    { TEXT_OPT_DEADZONE },
    { TEXT_OPT_RUMBLE },
};

static const u8 bindKeyNames[][32] = {
    { TEXT_KB_1 },
    { TEXT_KB_2 },
    { TEXT_KB_3 },
    { TEXT_KB_4 },
    { TEXT_KB_5 },
    { TEXT_KB_6 },
    { TEXT_KB_7 },
    { TEXT_KB_8 },
    { TEXT_KB_9 },
    { TEXT_KB_10 },
    { TEXT_KB_11 },
    { TEXT_KB_12 },
    { TEXT_KB_13 },
    { TEXT_KB_14 },
    { TEXT_KB_15 },
    { TEXT_KB_16 },
    { TEXT_KB_17 },
    { TEXT_KB_18 },
    { TEXT_KB_19 },
    { TEXT_KB_20 },
    { TEXT_KB_21 },
    { TEXT_KB_22 },
    { TEXT_KB_23 },
    { TEXT_KB_24 },
    { TEXT_KB_25 },
    { TEXT_KB_26 },
    { TEXT_KB_27 },
    { TEXT_KB_28 },
    { TEXT_KB_29 },
    { TEXT_KB_30 },
    { TEXT_KB_31 },
    { TEXT_KB_32 },
    { TEXT_KB_33 },
    { TEXT_KB_34 },
    { TEXT_KB_35 },
    { TEXT_KB_36 },
    { TEXT_KB_37 },
    { TEXT_KB_38 },
    { TEXT_KB_39 },
    { TEXT_KB_40 },
    { TEXT_KB_41 },
    { TEXT_KB_42 },
    { TEXT_KB_43 },
    { TEXT_KB_44 },
    { TEXT_KB_45 },
    { TEXT_KB_46 },
    { TEXT_KB_47 },
    { TEXT_KB_48 },
    { TEXT_KB_49 },
    { TEXT_KB_50 },
    { TEXT_KB_51 },
    { TEXT_KB_52 },
    { TEXT_KB_53 },
    { TEXT_KB_54 },
    { TEXT_KB_55 },
    { TEXT_KB_56 },
    { TEXT_KB_57 },
    { TEXT_KB_58 },
    { TEXT_KB_59 },
    { TEXT_KB_60 },
    { TEXT_KB_61 },
    { TEXT_KB_62 },
    { TEXT_KB_63 },
    { TEXT_KB_64 },
    { TEXT_KB_65 },
    { TEXT_KB_66 },
    { TEXT_KB_67 },
    { TEXT_KB_68 },
    { TEXT_KB_69 },
    { TEXT_KB_70 },
    { TEXT_KB_71 },
    { TEXT_KB_72 },
    { TEXT_KB_73 },
    { TEXT_KB_74 },
    { TEXT_KB_75 },
    { TEXT_KB_76 },
    { TEXT_KB_77 },
    { TEXT_KB_78 },
    { TEXT_KB_79 },
    { TEXT_KB_80 },
    { TEXT_KB_81 },
    { TEXT_KB_82 },
    { TEXT_KB_83 },
    { TEXT_KB_84 },
    { TEXT_KB_85 },
    { TEXT_KB_86 },
    { TEXT_KB_87 },
    { TEXT_KB_88 },
    { TEXT_KB_89 },
    { TEXT_KB_90 },
    { TEXT_KB_91 },
    { TEXT_KB_92 },
    { TEXT_KB_93 },
    { TEXT_KB_94 },
    { TEXT_KB_95 },
    { TEXT_KB_96 },
    { TEXT_KB_97 },
    { TEXT_KB_98 },
    { TEXT_KB_99 },
    { TEXT_KB_100 },
    { TEXT_KB_101 },
    { TEXT_KB_102 },
    { TEXT_KB_103 },
    { TEXT_KB_104 },
    { TEXT_KB_105 },
    { TEXT_KB_106 },
    { TEXT_KB_107 },
    { TEXT_KB_108 },
    { TEXT_KB_109 },
    { TEXT_KB_110 },
    { TEXT_KB_111 },
    { TEXT_KB_112 },
    { TEXT_KB_113 },
    { TEXT_KB_114 },
    { TEXT_KB_115 },
    { TEXT_KB_116 },
    { TEXT_KB_117 },
    { TEXT_KB_118 },
    { TEXT_KB_119 },
    { TEXT_KB_120 },
    { TEXT_KB_121 },
    { TEXT_KB_122 },
    { TEXT_KB_123 },
    { TEXT_KB_124 },
    { TEXT_KB_125 },
    { TEXT_KB_126 },
    { TEXT_KB_127 },
    { TEXT_KB_128 },
    { TEXT_KB_129 },
    { TEXT_KB_130 },
    { TEXT_KB_131 },
    { TEXT_KB_132 },
    { TEXT_KB_133 },
    { TEXT_KB_134 },
    { TEXT_KB_135 },
    { TEXT_KB_136 },
    { TEXT_KB_137 },
    { TEXT_KB_138 },
    { TEXT_KB_139 },
    { TEXT_KB_140 },
    { TEXT_KB_141 },
    { TEXT_KB_142 },
    { TEXT_KB_143 },
    { TEXT_KB_144 },
    { TEXT_KB_145 },
    { TEXT_KB_146 },
    { TEXT_KB_147 },
    { TEXT_KB_148 },
    { TEXT_KB_149 },
    { TEXT_KB_150 },
    { TEXT_KB_151 },
    { TEXT_KB_152 },
    { TEXT_KB_153 },
    { TEXT_KB_154 },
    { TEXT_KB_155 },
    { TEXT_KB_156 },
    { TEXT_KB_157 },
    { TEXT_KB_158 },
    { TEXT_KB_159 },
    { TEXT_KB_160 },
    { TEXT_KB_161 },
    { TEXT_KB_162 },
    { TEXT_KB_163 },
    { TEXT_KB_164 },
    { TEXT_KB_165 },
    { TEXT_KB_166 },
    { TEXT_KB_167 },
    { TEXT_KB_168 },
    { TEXT_KB_169 },
    { TEXT_KB_170 },
    { TEXT_KB_171 },
    { TEXT_KB_172 },
    { TEXT_KB_173 },
    { TEXT_KB_174 },
    { TEXT_KB_175 },
    { TEXT_KB_176 },
    { TEXT_KB_177 },
    { TEXT_KB_178 },
    { TEXT_KB_179 },
    { TEXT_KB_180 },
    { TEXT_KB_181 },
    { TEXT_KB_182 },
    { TEXT_KB_183 },
    { TEXT_KB_184 },
    { TEXT_KB_185 },
    { TEXT_KB_186 },
    { TEXT_KB_187 },
    { TEXT_KB_188 },
    { TEXT_KB_189 },
    { TEXT_KB_190 },
    { TEXT_KB_191 },
    { TEXT_KB_192 },
    { TEXT_KB_193 },
    { TEXT_KB_194 },
    { TEXT_KB_195 },
    { TEXT_KB_196 },
    { TEXT_KB_197 },
    { TEXT_KB_198 },
    { TEXT_KB_199 },
    { TEXT_KB_200 },
    { TEXT_KB_201 },
    { TEXT_KB_202 },
    { TEXT_KB_203 },
    { TEXT_KB_204 },
    { TEXT_KB_205 },
    { TEXT_KB_206 },
    { TEXT_KB_207 },
    { TEXT_KB_208 },
    { TEXT_KB_209 },
    { TEXT_KB_210 },
    { TEXT_KB_211 },
    { TEXT_KB_212 },
    { TEXT_KB_213 },
    { TEXT_KB_214 },
    { TEXT_KB_215 },
    { TEXT_KB_216 },
    { TEXT_KB_217 },
    { TEXT_KB_218 },
    { TEXT_KB_219 },
    { TEXT_KB_220 },
    { TEXT_KB_221 },
    { TEXT_KB_222 },
    { TEXT_KB_223 },
    { TEXT_KB_224 },
    { TEXT_KB_225 },
    { TEXT_KB_226 },
    { TEXT_KB_227 },
    { TEXT_KB_228 },
    { TEXT_KB_229 },
    { TEXT_KB_230 },
    { TEXT_KB_231 },
    { TEXT_KB_232 },
    { TEXT_KB_233 },
    { TEXT_KB_234 },
    { TEXT_KB_235 },
    { TEXT_KB_236 },
    { TEXT_KB_237 },
    { TEXT_KB_238 },
    { TEXT_KB_239 },
    { TEXT_KB_240 },
    { TEXT_KB_241 },
    { TEXT_KB_242 },
    { TEXT_KB_243 },
    { TEXT_KB_244 },
    { TEXT_KB_245 },
    { TEXT_KB_246 },
    { TEXT_KB_247 },
    { TEXT_KB_248 },
    { TEXT_KB_249 },
    { TEXT_KB_250 },
    { TEXT_KB_251 },
    { TEXT_KB_252 },
    { TEXT_KB_253 },
    { TEXT_KB_254 },
    { TEXT_KB_255 },
    { TEXT_KB_256 },
    { TEXT_KB_257 },
    { TEXT_KB_258 },
    { TEXT_KB_259 },
    { TEXT_KB_260 },
    { TEXT_KB_261 },
    { TEXT_KB_262 },
    { TEXT_KB_263 },
    { TEXT_KB_264 },
    { TEXT_KB_265 },
    { TEXT_KB_266 },
    { TEXT_KB_267 },
    { TEXT_KB_268 },
    { TEXT_KB_269 },
    { TEXT_KB_270 },
    { TEXT_KB_271 },
    { TEXT_KB_272 },
    { TEXT_KB_273 },
    { TEXT_KB_274 },
    { TEXT_KB_275 },
    { TEXT_KB_276 },
    { TEXT_KB_277 },
    { TEXT_KB_278 },
    { TEXT_KB_279 },
    { TEXT_KB_280 },
    { TEXT_KB_281 },
    { TEXT_KB_282 },
    { TEXT_KB_283 },
    { TEXT_KB_284 },
    { TEXT_KB_285 },
    { TEXT_KB_286 },
    { TEXT_KB_287 },
    { TEXT_KB_288 },
    { TEXT_KB_289 },
    { TEXT_KB_290 },
    { TEXT_KB_291 },
    { TEXT_KB_292 },
    { TEXT_KB_293 },
    { TEXT_KB_294 },
    { TEXT_KB_295 },
    { TEXT_KB_296 },
    { TEXT_KB_297 },
    { TEXT_KB_298 },
    { TEXT_KB_299 },
    { TEXT_KB_300 },
    { TEXT_KB_301 },
    { TEXT_KB_302 },
    { TEXT_KB_303 },
    { TEXT_KB_304 },
    { TEXT_KB_305 },
    { TEXT_KB_306 },
    { TEXT_KB_307 },
    { TEXT_KB_308 },
    { TEXT_KB_309 },
    { TEXT_KB_310 },
    { TEXT_KB_311 },
    { TEXT_KB_312 },
    { TEXT_KB_313 },
    { TEXT_KB_314 },
    { TEXT_KB_315 },
    { TEXT_KB_316 },
    { TEXT_KB_317 },
    { TEXT_KB_318 },
    { TEXT_KB_319 },
    { TEXT_KB_320 },
    { TEXT_KB_321 },
    { TEXT_KB_322 },
    { TEXT_KB_323 },
    { TEXT_KB_324 },
    { TEXT_KB_325 },
    { TEXT_KB_326 },
    { TEXT_KB_327 },
    { TEXT_KB_328 },
    { TEXT_KB_329 },
    { TEXT_KB_330 },
    { TEXT_KB_331 },
    { TEXT_KB_332 },
    { TEXT_KB_333 },
    { TEXT_KB_334 },
    { TEXT_KB_335 },
    { TEXT_KB_336 },
    { TEXT_KB_337 },
    { TEXT_KB_338 },
    { TEXT_KB_339 },
    { TEXT_KB_340 },
    { TEXT_KB_341 },
    { TEXT_KB_342 },
};

static const u8 *filterChoices[] = {
    optsVideoStr[3],
    optsVideoStr[4],
    //optsVideoStr[11],
};

static const u8 *vsyncChoices[] = {
    toggleStr[0],
    toggleStr[1],
    optsVideoStr[7],
};

enum OptType {
    OPT_INVALID = 0,
    OPT_TOGGLE,
    OPT_CHOICE,
    OPT_SCROLL,
    OPT_SUBMENU,
    OPT_BIND,
    OPT_BUTTON,
};

struct SubMenu;

struct Option {
    enum OptType type;
    const u8 *label;
    union {
        u32 *uval;
        bool *bval;
    };
    union {
        struct {
            const u8 **choices;
            u32 numChoices;
        };
        struct {
            u32 scrMin;
            u32 scrMax;
            u32 scrStep;
        };
        struct SubMenu *nextMenu;
        void (*actionFn)(struct Option *, s32);
    };
};

struct SubMenu {
    struct SubMenu *prev; // this is set at runtime to avoid needless complication
    const u8 *label;
    struct Option *opts;
    s32 numOpts;
    s32 select;
    s32 scroll;
};

/* helper macros */

#define DEF_OPT_TOGGLE(lbl, bv) \
    { .type = OPT_TOGGLE, .label = lbl, .bval = bv }
#define DEF_OPT_SCROLL(lbl, uv, min, max, st) \
    { .type = OPT_SCROLL, .label = lbl, .uval = uv, .scrMin = min, .scrMax = max, .scrStep = st }
#define DEF_OPT_CHOICE(lbl, uv, ch) \
    { .type = OPT_CHOICE, .label = lbl, .uval = uv, .choices = ch, .numChoices = sizeof(ch) / sizeof(ch[0]) }
#define DEF_OPT_SUBMENU(lbl, nm) \
    { .type = OPT_SUBMENU, .label = lbl, .nextMenu = nm }
#define DEF_OPT_BIND(lbl, uv) \
    { .type = OPT_BIND, .label = lbl, .uval = uv }
#define DEF_OPT_BUTTON(lbl, act) \
    { .type = OPT_BUTTON, .label = lbl, .actionFn = act }
#define DEF_SUBMENU(lbl, opt) \
    { .label = lbl, .opts = opt, .numOpts = sizeof(opt) / sizeof(opt[0]) }

/* button action functions */

static void optmenu_act_exit(UNUSED struct Option *self, s32 arg) {
    if (!arg || arg) game_exit(); // only exit on A press and not directions
}

static void optmenu_act_leave(UNUSED struct Option *self, s32 arg) {
    if (!arg) optmenu_toggle();	
	unpause_game();
	fade_into_special_warp(-2, 0);
}

static void optvideo_reset_window(UNUSED struct Option *self, s32 arg) {
    if (!arg || arg) {
        // Restrict reset to A press and not directions
        configWindow.reset = true;
        configWindow.settings_changed = true;
    }
}

static void optvideo_apply(UNUSED struct Option *self, s32 arg) {
    if (!arg) configWindow.settings_changed = true;
}

/* submenu option lists */

#ifdef BETTERCAMERA
static struct Option optsCamera[] = {
    DEF_OPT_TOGGLE( optsCameraStr[9], &configEnableCamera ),
    //DEF_OPT_TOGGLE( optsCameraStr[6], &configCameraAnalog ),
    DEF_OPT_TOGGLE( optsCameraStr[7], &configCameraMouse ),
    DEF_OPT_TOGGLE( optsCameraStr[10],&configCameraFOV ),
    DEF_OPT_TOGGLE( optsCameraStr[2], &configCameraInvertX ),
    DEF_OPT_TOGGLE( optsCameraStr[3], &configCameraInvertY ),
    DEF_OPT_SCROLL( optsCameraStr[0], &configCameraXSens, 1, 100, 1 ),
    DEF_OPT_SCROLL( optsCameraStr[1], &configCameraYSens, 1, 100, 1 ),
    DEF_OPT_SCROLL( optsCameraStr[4], &configCameraAggr, 0, 100, 1 ),
    DEF_OPT_SCROLL( optsCameraStr[5], &configCameraPan, 0, 100, 1 ),
    DEF_OPT_SCROLL( optsCameraStr[8], &configCameraDegrade, 0, 100, 1 ),
};
#endif

static struct Option optsControls[] = {
    DEF_OPT_BIND( bindStr[ 2], configKeyA ),
    DEF_OPT_BIND( bindStr[ 3], configKeyB ),
    DEF_OPT_BIND( bindStr[ 4], configKeyStart ),
    DEF_OPT_BIND( bindStr[ 5], configKeyL ),
    DEF_OPT_BIND( bindStr[ 6], configKeyR ),
    DEF_OPT_BIND( bindStr[ 7], configKeyZ ),
    DEF_OPT_BIND( bindStr[ 8], configKeyCUp ),
    DEF_OPT_BIND( bindStr[ 9], configKeyCDown ),
    DEF_OPT_BIND( bindStr[10], configKeyCLeft ),
    DEF_OPT_BIND( bindStr[11], configKeyCRight ),
    DEF_OPT_BIND( bindStr[12], configKeyStickUp ),
    DEF_OPT_BIND( bindStr[13], configKeyStickDown ),
    DEF_OPT_BIND( bindStr[14], configKeyStickLeft ),
    DEF_OPT_BIND( bindStr[15], configKeyStickRight ),
    DEF_OPT_BIND( bindStr[16], configKeyMouseToStick ),
    DEF_OPT_BIND( bindStr[17], configKeyWalk ),
    DEF_OPT_SCROLL( bindStr[18], &configSpeed, 0, 10, 1 ),
    // max deadzone is 31000; this is less than the max range of ~32768, but this
    // way, the player can't accidentally lock themselves out of using the stick
    DEF_OPT_SCROLL( bindStr[19], &configStickDeadzone, 0, 100, 1 ),
    DEF_OPT_SCROLL( bindStr[20], &configRumbleStrength, 0, 100, 1)
};

static struct Option optsVideo[] = {
    DEF_OPT_TOGGLE( optsVideoStr[0], &configWindow.fullscreen ),
    DEF_OPT_TOGGLE( optsVideoStr[6], &configWindow.vsync ),
    DEF_OPT_CHOICE( optsVideoStr[1], &configFiltering, filterChoices ),
    DEF_OPT_TOGGLE( optsVideoStr[2], &configAntiAliasing ),
    DEF_OPT_TOGGLE( optsVideoStr[9], &configFrameskip ),
#ifndef NODRAWINGDISTANCE
    DEF_OPT_SCROLL( optsVideoStr[12], &configDrawDistance, 50, 509, 10 ),
#endif
    DEF_OPT_TOGGLE( optsVideoStr[8], &configInputDisplay ),
    DEF_OPT_TOGGLE( optsVideoStr[10], &configHUD ),
    DEF_OPT_BUTTON( optsVideoStr[5], optvideo_reset_window ),
};

static struct Option optsAudio[] = {
    DEF_OPT_SCROLL( optsAudioStr[0], &configMasterVolume, 0, MAX_VOLUME, 1 ),
    DEF_OPT_SCROLL( optsAudioStr[1], &configMusicVolume, 0, MAX_VOLUME, 1),
    DEF_OPT_SCROLL( optsAudioStr[2], &configSfxVolume, 0, MAX_VOLUME, 1),
    DEF_OPT_SCROLL( optsAudioStr[3], &configEnvVolume, 0, MAX_VOLUME, 1),
};

static struct Option optsCheats[] = {
    DEF_OPT_TOGGLE( optsCheatsStr[0], &Cheats.EnableCheats ),
    DEF_OPT_TOGGLE( optsCheatsStr[1], &Cheats.MoonJump ),
    DEF_OPT_TOGGLE( optsCheatsStr[2], &Cheats.GodMode ),
    DEF_OPT_TOGGLE( optsCheatsStr[3], &Cheats.InfiniteLives ),
    DEF_OPT_TOGGLE( optsCheatsStr[4], &Cheats.SuperSpeed ),
    DEF_OPT_TOGGLE( optsCheatsStr[5], &Cheats.Responsive ),
    DEF_OPT_TOGGLE( optsCheatsStr[6], &Cheats.ExitAnywhere ),
    DEF_OPT_TOGGLE( optsCheatsStr[7], &Cheats.HugeMario ),
    DEF_OPT_TOGGLE( optsCheatsStr[8], &Cheats.TinyMario ),

};

/* submenu definitions */

#ifdef BETTERCAMERA
static struct SubMenu menuCamera   = DEF_SUBMENU( menuStr[1], optsCamera );
#endif
static struct SubMenu menuControls = DEF_SUBMENU( menuStr[2], optsControls );
static struct SubMenu menuVideo    = DEF_SUBMENU( menuStr[3], optsVideo );
static struct SubMenu menuAudio    = DEF_SUBMENU( menuStr[4], optsAudio );
static struct SubMenu menuCheats   = DEF_SUBMENU( menuStr[6], optsCheats );

/* main options menu definition */

static struct Option optsMain[] = {
#ifdef BETTERCAMERA
    DEF_OPT_SUBMENU( menuStr[1], &menuCamera ),
#endif
    DEF_OPT_SUBMENU( menuStr[2], &menuControls ),
    DEF_OPT_SUBMENU( menuStr[3], &menuVideo ),
    DEF_OPT_SUBMENU( menuStr[4], &menuAudio ),
    DEF_OPT_BUTTON ( menuStr[5], optmenu_act_leave ),
    DEF_OPT_BUTTON ( menuStr[6], optmenu_act_exit ),
    // NOTE: always keep cheats the last option here because of the half-assed way I toggle them
    DEF_OPT_SUBMENU( menuStr[7], &menuCheats )
};

static struct SubMenu menuMain = DEF_SUBMENU( menuStr[0], optsMain );

/* implementation */

static s32 optmenu_option_timer = 0;
static u8 optmenu_hold_count = 0;

static struct SubMenu *currentMenu = &menuMain;

static inline s32 wrap_add(s32 a, const s32 b, const s32 min, const s32 max) {
    a += b;
    if (a < min) a = max - (min - a) + 1;
    else if (a > max) a = min + (a - max) - 1;
    return a;
}

static void uint_to_hex(u32 num, u8 *dst) {
    u8 places = 4;
    while (places--) {
        const u32 digit = num & 0xF;
        dst[places] = digit;
        num >>= 4;
    }
    dst[4] = DIALOG_CHAR_TERMINATOR;
}

//Displays a box.
static void optmenu_draw_box(s16 x1, s16 y1, s16 x2, s16 y2, u8 r, u8 g, u8 b) {
    gDPPipeSync(gDisplayListHead++);
    gDPSetRenderMode(gDisplayListHead++, G_RM_OPA_SURF, G_RM_OPA_SURF2);
    gDPSetCycleType(gDisplayListHead++, G_CYC_FILL);
    gDPSetFillColor(gDisplayListHead++, GPACK_RGBA5551(r, g, b, 255));
    gDPFillRectangle(gDisplayListHead++, x1, y1, x2 - 1, y2 - 1);
    gDPPipeSync(gDisplayListHead++);
    gDPSetCycleType(gDisplayListHead++, G_CYC_1CYCLE);
}

static void optmenu_draw_text(s16 x, s16 y, const u8 *str, u8 col) {
    const u8 textX = get_str_x_pos_from_center(x, (u8*)str, 10.0f);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 255);
    print_generic_string(textX+1, y-1, str);
    if (col == 3) {
        gDPSetEnvColor(gDisplayListHead++, 255, 215, 0, 255);
    } else if (col == 2) {
        gDPSetEnvColor(gDisplayListHead++, 170, 170, 100, 255);
    } else if (col == 0) {
        gDPSetEnvColor(gDisplayListHead++, 128, 128, 128, 255);
    } else {
        gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    }
    print_generic_string(textX, y, str);
}

static void optmenu_draw_opt(const struct Option *opt, s16 x, s16 y, u8 sel) {
    u8 buf[32] = { 0 };

    if (opt->type == OPT_SUBMENU || opt->type == OPT_BUTTON)
        y -= 6;

    optmenu_draw_text(x, y, opt->label, sel ? 3 : 2);

    switch (opt->type) {
        case OPT_TOGGLE:
            optmenu_draw_text(x, y-13, toggleStr[(int)*opt->bval], sel);
            break;

        case OPT_CHOICE:
            optmenu_draw_text(x, y-13, opt->choices[*opt->uval], sel);
            break;

        case OPT_SCROLL:
            int_to_str(*opt->uval, buf);
            optmenu_draw_text(x, y-13, buf, sel);
            break;

        case OPT_BIND:
            x = 112;
            for (u8 i = 0; i < MAX_BINDS; ++i, x += 48) {
                const u8 white = (sel && (optmenu_bind_idx == i));
                // TODO: button names
                if (opt->uval[i] == VK_INVALID) {
                    if (optmenu_binding && white)
                        optmenu_draw_text(x, y-13, bindStr[1], 1);
                    else
                        optmenu_draw_text(x, y-13, bindStr[0], white);
                } else if (opt->uval[i] < 340) {
                    optmenu_draw_text(x, y-13, bindKeyNames[opt->uval[i]-1], white);
                } else if (opt->uval[i] == 4353) {
                    optmenu_draw_text(x, y-13, bindKeyNames[340], white);
                } else if (opt->uval[i] == 4354) {
                    optmenu_draw_text(x, y-13, bindKeyNames[341], white);
                } else if (opt->uval[i] == 4355) {
                    optmenu_draw_text(x, y-13, bindKeyNames[342], white);
                } else {
                    uint_to_hex(opt->uval[i], buf);
                    optmenu_draw_text(x, y-13, buf, white);
                }
            }
            break;

        default: break;
    };
}

static void optmenu_opt_change(struct Option *opt, s32 val) {
    switch (opt->type) {
        case OPT_TOGGLE:
            *opt->bval = !*opt->bval;
            if (opt->label[0] == 15) configWindow.settings_changed = true;
            break;

        case OPT_CHOICE:
            if (opt->label[0] == 29) configWindow.aa_changed = true;
            *opt->uval = wrap_add(*opt->uval, val, 0, opt->numChoices - 1);
            break;

        case OPT_SCROLL:
            *opt->uval = wrap_add(*opt->uval, opt->scrStep * val, opt->scrMin, opt->scrMax);
            break;

        case OPT_SUBMENU:
            opt->nextMenu->prev = currentMenu;
            currentMenu = opt->nextMenu;
            break;

        case OPT_BUTTON:
            if (opt->actionFn)
                opt->actionFn(opt, val);
            break;

        case OPT_BIND:
            if (val == 0xFF) {
                // clear the bind
                opt->uval[optmenu_bind_idx] = VK_INVALID;
            } else if (val == 0) {
                opt->uval[optmenu_bind_idx] = VK_INVALID;
                optmenu_binding = 1;
                controller_get_raw_key(); // clear the last key, which is probably A
            } else {
                optmenu_bind_idx = wrap_add(optmenu_bind_idx, val, 0, MAX_BINDS - 1);
            }
            break;

        default: break;
    }
}

static inline s16 get_hudstr_centered_x(const s16 sx, const u8 *str) {
    const u8 *chr = str;
    s16 len = 0;
    while (*chr != GLOBAR_CHAR_TERMINATOR) ++chr, ++len;
    return sx - len * 6; // stride is 12
}

//Options menu
void optmenu_draw(void) {
    s16 scroll;
    s16 scrollpos;

    Mtx *matrix = (Mtx *) alloc_display_list(sizeof(Mtx));
    if (!matrix) return;
    create_dl_translation_matrix(MENU_MTX_PUSH, -1500, SCREEN_HEIGHT, 1);
    guScale(matrix, 30.f, 8.f, 0);
    gSPMatrix(gDisplayListHead++, VIRTUAL_TO_PHYSICAL(matrix), G_MTX_MODELVIEW | G_MTX_MUL | G_MTX_NOPUSH);
    gDPSetEnvColor(gDisplayListHead++, 0, 0, 0, 175);
    gSPDisplayList(gDisplayListHead++, dl_draw_text_bg_box);
    gSPPopMatrix(gDisplayListHead++, G_MTX_MODELVIEW);

    const s16 labelX = get_hudstr_centered_x(160, currentMenu->label);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_begin);
    gDPSetEnvColor(gDisplayListHead++, 255, 255, 255, 255);
    print_hud_lut_string(HUD_LUT_GLOBAL, labelX, 10, currentMenu->label);
    gSPDisplayList(gDisplayListHead++, dl_rgba16_text_end);

    if (currentMenu->numOpts > 7) {
        optmenu_draw_box(272, 38, 280, 228, 0x80, 0x80, 0x80);
        scrollpos = 64 * ((f32)currentMenu->scroll / (currentMenu->numOpts-7));
        optmenu_draw_box(272, 38+scrollpos, 280, 164+scrollpos, 0xAA, 0xAA, 0x64);
    }

    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    
    for (u8 i = 0; i < currentMenu->numOpts; i++) {
        scroll = 200 - 28 * i + currentMenu->scroll * 28;
        // FIXME: just start from the first visible option bruh
        if (scroll <= 200 && scroll > 28)
            optmenu_draw_opt(&currentMenu->opts[i], 160, scroll-10, (currentMenu->select == i));
    }

    gDPSetScissor(gDisplayListHead++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    gDPSetEnvColor(gDisplayListHead++, 255, 215, 0, 255);
    print_generic_string(72, 183 - (28 * (currentMenu->select - currentMenu->scroll)), optSmallStr[2]);
    print_generic_string(232, 183 - (28 * (currentMenu->select - currentMenu->scroll)), optSmallStr[3]);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

//This has been separated for interesting reasons. Don't question it.
void optmenu_draw_prompt(void) {
    gSPDisplayList(gDisplayListHead++, dl_ia_text_begin);
    optmenu_draw_text(264, 212, optSmallStr[optmenu_open], 1);
    gSPDisplayList(gDisplayListHead++, dl_ia_text_end);
}

void optmenu_toggle(void) {
    if (optmenu_open == 0) {
        #ifndef nosound
        play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
        #endif

        // HACK: hide the last option in main if cheats are disabled
        menuMain.numOpts = sizeof(optsMain) / sizeof(optsMain[0]);
        if (!Cheats.EnableCheats) {
            menuMain.numOpts--;
            if (menuMain.select >= menuMain.numOpts) {
                menuMain.select = 0; // don't bother
                menuMain.scroll = 0;
            }
        }

        currentMenu = &menuMain;
        optmenu_open = 1;
        /* Resets l_counter to 0 every time the options menu is open */
        l_counter = 0;
    } else {
        #ifndef nosound
        play_sound(SOUND_MENU_MARIO_CASTLE_WARP2, gDefaultSoundArgs);
        #endif
        optmenu_open = 0;
        #ifdef BETTERCAMERA
        newcam_init_settings(); // load bettercam settings from config vars
        #endif
        controller_reconfigure(); // rebind using new config values
        configfile_save(configfile_name());
    }
}

void optmenu_check_buttons(void) {
    if (optmenu_binding) {
        u32 key = controller_get_raw_key();
        if (key != VK_INVALID) {
            #ifndef nosound
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
            #endif
            currentMenu->opts[currentMenu->select].uval[optmenu_bind_idx] = key;
            optmenu_binding = 0;
            optmenu_option_timer = 12;
        }
        return;
    }

    if (gPlayer1Controller->buttonPressed & R_TRIG || pressed_r == 1)
        optmenu_toggle();

    /* Enables cheats if the user press the L trigger 3 times while in the options menu. Also plays a sound. */
    
    if ((gPlayer1Controller->buttonPressed & L_TRIG) && !Cheats.EnableCheats) {
        if (l_counter == 2) {
                Cheats.EnableCheats = true;
                play_sound(SOUND_MENU_STAR_SOUND, gDefaultSoundArgs);
                l_counter = 0;
        } else {
            l_counter++;
        }
    }
    
    if (!optmenu_open) return;

    u8 allowInput = 0;

    optmenu_option_timer--;
    if (optmenu_option_timer <= 0) {
        if (optmenu_hold_count == 0) {
            optmenu_hold_count++;
            optmenu_option_timer = 10;
        } else {
            optmenu_option_timer = 3;
        }
        allowInput = 1;
    }

    if (ABS(gPlayer1Controller->stickY) > 60 || pressed_up || pressed_down) {
        if (allowInput) {
            #ifndef nosound
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
            #endif

            if (gPlayer1Controller->stickY >= 60 || pressed_up == 1) {
                currentMenu->select--;
                if (currentMenu->select < 0)
                    currentMenu->select = currentMenu->numOpts-1;
            } else if (gPlayer1Controller->stickY < 60 || pressed_down) {
                currentMenu->select++;
                if (currentMenu->select >= currentMenu->numOpts)
                    currentMenu->select = 0;
            }

            if (currentMenu->select < currentMenu->scroll)
                currentMenu->scroll = currentMenu->select;
            else if (currentMenu->select > currentMenu->scroll + 6)
                currentMenu->scroll = currentMenu->select - 6;
        }
    } else if (ABS(gPlayer1Controller->stickX) > 60 || pressed_left || pressed_right) {
        struct Option *opt = &currentMenu->opts[currentMenu->select];
        if (allowInput && opt->type != OPT_SUBMENU && opt->type != OPT_BUTTON) {
            #ifndef nosound
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
            #endif
            if (gPlayer1Controller->stickX >= 60 || pressed_right)
                optmenu_opt_change(&currentMenu->opts[currentMenu->select], 1);
            else if (gPlayer1Controller->stickX < 60 || pressed_left)
                optmenu_opt_change(&currentMenu->opts[currentMenu->select], -1);
        }
    } else if (gPlayer1Controller->buttonPressed & A_BUTTON || pressed_enter == 1) {
        struct Option *opt = &currentMenu->opts[currentMenu->select];
        if (allowInput) {
            #ifndef nosound
            play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
            #endif
            optmenu_opt_change(&currentMenu->opts[currentMenu->select], opt->type == OPT_BUTTON ? 1 : 0);
        }
    } else if (gPlayer1Controller->buttonPressed & B_BUTTON || pressed_esc == 1) {
        if (allowInput) {
            if (currentMenu->prev) {
                #ifndef nosound
                play_sound(SOUND_MENU_CHANGE_SELECT, gDefaultSoundArgs);
                #endif
                currentMenu = currentMenu->prev;
            } else {
                // can't go back, exit the menu altogether
                optmenu_toggle();
            }
        }
    } else if (gPlayer1Controller->buttonPressed & Z_TRIG || pressed_del) {
        // HACK: clear binds with Z
        if (allowInput && currentMenu->opts[currentMenu->select].type == OPT_BIND)
            optmenu_opt_change(&currentMenu->opts[currentMenu->select], 0xFF);
    /*} else if (gPlayer1Controller->buttonPressed & START_BUTTON) {
        if (allowInput) optmenu_toggle();*/
    } else {
        optmenu_hold_count = 0;
        optmenu_option_timer = 0;
    }
}

#endif // EXT_OPTIONS_MENU
