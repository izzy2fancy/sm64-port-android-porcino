// ddd_sub.c.inc

void bhv_bowsers_sub_loop(void) {
    if ((save_file_get_flags() & (SAVE_FLAG_HAVE_KEY_2 | SAVE_FLAG_UNLOCKED_UPSTAIRS_DOOR)) && !(gCurrActNum == 1 && save_file_get_total_star_count(gCurrSaveFileNum - 1, 0, 24) == 120))
        obj_mark_for_deletion(o);
}
