#pragma once

#define EE_HANDS

#define OLED_DISABLE_TIMEOUT
#define OLED_DISPLAY_128X64

#ifdef RGBLIGHT_ENABLE
#    define RGBLIGHT_SLEEP
#    define RGBLIGHT_LIMIT_VAL 200
#    define RGBLIGHT_HUE_STEP 8
#    define RGBLIGHT_SAT_STEP 8
#    define RGBLIGHT_VAL_STEP 8
#    define RGBLIGHT_SPLIT
#endif
