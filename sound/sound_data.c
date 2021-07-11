#include "platform_info.h"

unsigned char gSoundDataADSR[] = {
#if IS_BIG_ENDIAN && IS_64_BIT
#include "sound/sound_data.ctl.be.64.inc.c"
#elif IS_BIG_ENDIAN && !IS_64_BIT
#include "sound/sound_data.ctl.be.32.inc.c"
#elif !IS_BIG_ENDIAN && IS_64_BIT
#include "sound/sound_data.ctl.le.64.inc.c"
#elif !IS_BIG_ENDIAN && !IS_64_BIT
#include "sound/sound_data.ctl.le.32.inc.c"
#endif
};

unsigned char gSoundDataRaw[] = {
#if IS_BIG_ENDIAN && IS_64_BIT
#include "sound/sound_data.tbl.be.64.inc.c"
#elif IS_BIG_ENDIAN && !IS_64_BIT
#include "sound/sound_data.tbl.be.32.inc.c"
#elif !IS_BIG_ENDIAN && IS_64_BIT
#include "sound/sound_data.tbl.le.64.inc.c"
#elif !IS_BIG_ENDIAN && !IS_64_BIT
#include "sound/sound_data.tbl.le.32.inc.c"
#endif
};

unsigned char gMusicData[] = {
#if IS_BIG_ENDIAN && IS_64_BIT
#include "sound/sequences.bin.be.64.inc.c"
#elif IS_BIG_ENDIAN && !IS_64_BIT
#include "sound/sequences.bin.be.32.inc.c"
#elif !IS_BIG_ENDIAN && IS_64_BIT
#include "sound/sequences.bin.le.64.inc.c"
#elif !IS_BIG_ENDIAN && !IS_64_BIT
#include "sound/sequences.bin.le.32.inc.c"
#endif
};

unsigned char gBankSetsData[] = {
#if IS_BIG_ENDIAN && IS_64_BIT
#include "sound/bank_sets.be.64.inc.c"
#elif IS_BIG_ENDIAN && !IS_64_BIT
#include "sound/bank_sets.be.32.inc.c"
#elif !IS_BIG_ENDIAN && IS_64_BIT
#include "sound/bank_sets.le.64.inc.c"
#elif !IS_BIG_ENDIAN && !IS_64_BIT
#include "sound/bank_sets.le.32.inc.c"
#endif
};
