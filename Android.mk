LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE := main

SDL_PATH := ../SDL

### Build Options ###

# These options can be changed by modifying the makefile.

# Version of the game to build
VERSION ?= us

# Use touch controls
TOUCH_CONTROLS ?= 1

# Enable extended options menu by default
EXT_OPTIONS_MENU ?= 1

ifeq ($(VERSION),jp)
  VERSION_DEF := VERSION_JP
else
ifeq ($(VERSION),us)
  VERSION_DEF := VERSION_US
else
ifeq ($(VERSION),eu)
  VERSION_DEF := VERSION_EU
else
ifeq ($(VERSION),sh)
  $(warning Building SH is experimental and is prone to breaking. Try at your own risk.)
  VERSION_DEF := VERSION_SH
else
  $(error unknown version "$(VERSION)")
endif
endif
endif
endif

PC_BUILD_DIR := $(LOCAL_PATH)/build/$(VERSION)_pc

SRC_DIRS := src src/engine src/game src/audio src/menu src/buffers actors levels bin bin/$(VERSION) data assets src/pc src/pc/gfx src/pc/audio src/pc/controller src/pc/fs
SRC_DIRS := $(addprefix $(LOCAL_PATH)/,$(SRC_DIRS))

GODDARD_SRC_DIRS := src/goddard src/goddard/dynlists
GODDARD_SRC_DIRS := $(addprefix $(LOCAL_PATH)/,$(GODDARD_SRC_DIRS))

ULTRA_SRC_DIRS := lib/src lib/src/math
ULTRA_SRC_DIRS := $(addprefix $(LOCAL_PATH)/,$(ULTRA_SRC_DIRS))

# Source code files
LEVEL_C_FILES := $(wildcard $(LOCAL_PATH)/levels/*/leveldata.c) $(wildcard $(LOCAL_PATH)/levels/*/script.c) $(wildcard $(LOCAL_PATH)/levels/*/geo.c)
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c)) $(LEVEL_C_FILES)

ULTRA_C_FILES := \
  alBnkfNew.c \
  guLookAtRef.c \
  guMtxF2L.c \
  guNormalize.c \
  guOrthoF.c \
  guPerspectiveF.c \
  guRotateF.c \
  guScaleF.c \
  guTranslateF.c \
  ldiv.c

C_FILES := $(filter-out $(LOCAL_PATH)/src/game/main.c,$(C_FILES))
ULTRA_C_FILES := $(addprefix $(LOCAL_PATH)/lib/src/,$(ULTRA_C_FILES))

GODDARD_C_FILES := $(foreach dir,$(GODDARD_SRC_DIRS),$(wildcard $(dir)/*.c))

GENERATED_C_FILES := $(PC_BUILD_DIR)/assets/mario_anim_data.c $(PC_BUILD_DIR)/assets/demo_data.c \
  $(addprefix $(PC_BUILD_DIR)/bin/,$(addsuffix _skybox.c,$(notdir $(basename $(wildcard $(LOCAL_PATH)/textures/skyboxes/*.png)))))

LOCAL_SHARED_LIBRARIES := SDL2
LOCAL_LDLIBS := -lEGL -lGLESv2 -llog
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/include $(LOCAL_PATH)/src $(PC_BUILD_DIR) $(PC_BUILD_DIR)/include
LOCAL_CFLAGS := -DNON_MATCHING -DAVOID_UB -DTARGET_LINUX -DENABLE_OPENGL -DWIDESCREEN -DF3DEX_GBI_2E -D_LANGUAGE_C -DNO_SEGMENTED_MEMORY -D$(VERSION_DEF) -DSTDC_HEADERS -DUSE_GLES -DRAPI_GL -DWAPI_SDL2 -DAAPI_SDL2 -DCAPI_SDL2 -DHAVE_SDL2

ifeq ($(TOUCH_CONTROLS),1)
  LOCAL_CFLAGS += -DTOUCH_CONTROLS
endif

# Check for extended options menu option
ifeq ($(EXT_OPTIONS_MENU),1)
  LOCAL_CFLAGS += -DEXT_OPTIONS_MENU
endif

LOCAL_SRC_FILES := $(C_FILES) $(GENERATED_C_FILES) $(ULTRA_C_FILES) $(GODDARD_C_FILES) $(LOCAL_PATH)/sound/sound_data.c

include $(BUILD_SHARED_LIBRARY)
