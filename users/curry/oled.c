#include "curry.h"

#ifdef OLED_DISPLAY_128X64
#   define KEYLOGGER_LENGTH 10
#else
#   define KEYLOGGER_LENGTH 5
#endif

static uint32_t oled_timer                       = 0;
static char     keylog_str[KEYLOGGER_LENGTH + 1] = {"\n"};
static uint16_t log_timer                        = 0;
// clang-format off
static const char PROGMEM code_to_name[0xFF] = {
//   0    1    2    3    4    5    6    7    8    9    A    B    c    D    E    F
    ' ', ' ', ' ', ' ', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',  // 0x
    'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2',  // 1x
    '3', '4', '5', '6', '7', '8', '9', '0',  20,  19,  27,  26,  22, '-', '=', '[',  // 2x
    ']','\\', '#', ';','\'', '`', ',', '.', '/', 128, ' ', ' ', ' ', ' ', ' ', ' ',  // 3x
    ' ', ' ', ' ', ' ', ' ', ' ', 'P', 'S', ' ', ' ', ' ', ' ',  16, ' ', ' ', ' ',  // 4x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 5x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 6x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 7x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 8x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // 9x
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Ax
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Bx
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Cx
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Dx
    'C', 'S', 'A', 'C', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',  // Ex
    ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '        // Fx
};

// clang-format on
void add_keylog(uint16_t keycode);

oled_rotation_t oled_init_user(oled_rotation_t rotation) { return OLED_ROTATION_270; }

void add_keylog(uint16_t keycode) {
    if ((keycode >= QK_MOD_TAP && keycode <= QK_MOD_TAP_MAX) || (keycode >= QK_LAYER_TAP && keycode <= QK_LAYER_TAP_MAX) || (keycode >= QK_MODS && keycode <= QK_MODS_MAX)) {
        keycode = keycode & 0xFF;
    } else if (keycode > 0xFF) {
        keycode = 0;
    }

    for (uint8_t i = (KEYLOGGER_LENGTH - 1); i > 0; --i) {
        keylog_str[i] = keylog_str[i - 1];
    }

    if (keycode < (sizeof(code_to_name) / sizeof(char))) {
        keylog_str[0] = pgm_read_byte(&code_to_name[keycode]);
    }

    log_timer = timer_read();
}

void render_keylogger_status(void) {
#if defined(OLED_DISPLAY_128X64)
    oled_write_P(PSTR("Keylogger: "), false);
#else
    oled_write_P(PSTR("Keys:"), false);
#endif
    oled_write(keylog_str, false);
}


void render_default_layer_state(void) {
#if defined(OLED_DISPLAY_128X64)
    oled_write_P(PSTR("Layout: "), false);
    switch (get_highest_layer(default_layer_state)) {
        case _QWERTY:
            oled_write_ln_P(PSTR("Qwerty"), false);
            break;
        case _COLEMAK:
            oled_write_ln_P(PSTR("Colemak"), false);
            break;
        case _DVORAK:
            oled_write_ln_P(PSTR("Dvorak"), false);
            break;
        case _WORKMAN:
            oled_write_ln_P(PSTR("Workman"), false);
            break;
    }
#else
    oled_write_P(PSTR("Lyout"), false);
    switch (get_highest_layer(default_layer_state)) {
        case _QWERTY:
            oled_write_P(PSTR(" QRTY"), false);
            break;
        case _COLEMAK:
            oled_write_P(PSTR(" COLE"), false);
            break;
        case _DVORAK:
            oled_write_P(PSTR(" DVRK"), false);
            break;
        case _WORKMAN:
            oled_write_P(PSTR(" WRKM"), false);
            break;
    }
#endif
}


void render_layer_state(void) {
#if defined(OLED_DISPLAY_128X64)
    oled_write_ln_P(PSTR("Layer:"), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("Lower"), layer_state_is(_LOWER));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("Raise"), layer_state_is(_RAISE));
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR("Mods"), layer_state_is(_MODS));
#else
    oled_write_P(PSTR("LAYER"), false);
    oled_write_P(PSTR("Lower"), layer_state_is(_LOWER));
    oled_write_P(PSTR("Raise"), layer_state_is(_RAISE));
    oled_write_P(PSTR(" Mods"), layer_state_is(_MODS));
#endif
}

void render_keylock_status(uint8_t led_usb_state) {
#if defined(OLED_DISPLAY_128X64)
    oled_write_P(PSTR("Lock: "), false);
    oled_write_P(PSTR("NUML"), led_usb_state & (1 << USB_LED_NUM_LOCK));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("CAPS"), led_usb_state & (1 << USB_LED_CAPS_LOCK));
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR("SCLK"), led_usb_state & (1 << USB_LED_SCROLL_LOCK));
#else
    oled_write_P(PSTR("Lock:"), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("N"), led_usb_state & (1 << USB_LED_NUM_LOCK));
    oled_write_P(PSTR("C"), led_usb_state & (1 << USB_LED_CAPS_LOCK));
    oled_write_ln_P(PSTR("S"), led_usb_state & (1 << USB_LED_SCROLL_LOCK));
#endif
}

void render_mod_status(uint8_t modifiers) {
#if defined(OLED_DISPLAY_128X64)
    oled_write_P(PSTR("Mods: "), false);
    oled_write_P(PSTR("Sft"), (modifiers & MOD_MASK_SHIFT));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("Ctl"), (modifiers & MOD_MASK_CTRL));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("Alt"), (modifiers & MOD_MASK_ALT));
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("GUI"), (modifiers & MOD_MASK_GUI));
#else
    oled_write_P(PSTR("Mods:"), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("S"), (modifiers & MOD_MASK_SHIFT));
    oled_write_P(PSTR("C"), (modifiers & MOD_MASK_CTRL));
    oled_write_P(PSTR("A"), (modifiers & MOD_MASK_ALT));
    oled_write_P(PSTR("G"), (modifiers & MOD_MASK_GUI));
#endif
}

void render_bootmagic_status(void) {
    /* Show Ctrl-Gui Swap options */
    static const char PROGMEM logo[][2][3] = {
        {{0x97, 0x98, 0}, {0xb7, 0xb8, 0}},
        {{0x95, 0x96, 0}, {0xb5, 0xb6, 0}},
    };
#if defined(OLED_DISPLAY_128X64)
    oled_write_P(PSTR("Boot  "), false);
    if (keymap_config.swap_lctl_lgui) {
        oled_write_P(logo[1][0], false);
    } else {
        oled_write_P(logo[0][0], false);
    }
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("NKRO"), keymap_config.nkro);
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR("GUI"), !keymap_config.no_gui);
    oled_write_P(PSTR("Magic "), false);
    if (keymap_config.swap_lctl_lgui) {
        oled_write_P(logo[1][1], false);
    } else {
        oled_write_P(logo[0][1], false);
    }
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("GRV"), keymap_config.swap_grave_esc);
    oled_write_P(PSTR("  "), false);
    oled_write_ln_P(PSTR("CAPS"), keymap_config.swap_control_capslock);
#else
    oled_write_P(PSTR("BTMGK"), false);
    oled_write_P(PSTR(" "), false);
    oled_write_P(logo[0][0], !keymap_config.swap_lctl_lgui);
    oled_write_P(logo[1][0], keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR(" "), false);
    oled_write_P(logo[0][1], !keymap_config.swap_lctl_lgui);
    oled_write_P(logo[1][1], keymap_config.swap_lctl_lgui);
    oled_write_P(PSTR(" NKRO"), keymap_config.nkro);
#endif
}

void render_user_status(void) {
#if defined(OLED_DISPLAY_128X64)
    oled_write_P(PSTR("USER: "), false);
    oled_write_P(PSTR("Anim"), userspace_config.rgb_matrix_idle_anim);
    oled_write_P(PSTR(" "), false);
    oled_write_P(PSTR("Layr"), userspace_config.rgb_layer_change);
    oled_write_P(PSTR(" "), false);
    oled_write_ln_P(PSTR("Nuke"), userspace_config.nuke_switch);
#else
    oled_write_P(PSTR("USER:"), false);
    oled_write_P(PSTR(" Anim"), userspace_config.rgb_matrix_idle_anim);
    oled_write_P(PSTR(" Layr"), userspace_config.rgb_layer_change);
    oled_write_P(PSTR(" Nuke"), userspace_config.nuke_switch);
#endif
}

// clang-format off
void render_logo(void) {
    static const char PROGMEM qmk_logo[] = {
        0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,0x8f,0x90,0x91,0x92,0x93,0x94,
        0xa0,0xa1,0xa2,0xa3,0xa4,0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,0xb0,0xb1,0xb2,0xb3,0xb4,
        0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,0xd1,0xd2,0xd3,0xd4,0};

    oled_write_P(qmk_logo, false);
}

void render_status_main(void) {
    /* Show Keyboard Layout  */
    render_default_layer_state();
    render_keylock_status(host_keyboard_leds());
    render_bootmagic_status();
    render_user_status();

    render_keylogger_status();
}

void render_status_secondary(void) {
    /* Show Keyboard Layout  */
#ifdef OLED_DISPLAY_128X64
    render_logo();
#endif
    render_default_layer_state();
    render_layer_state();
    render_mod_status(get_mods() | get_oneshot_mods());

    render_keylogger_status();
}

void oled_task_user(void) {
    if (is_keyboard_master()) {
        if (timer_elapsed32(oled_timer) > 30000) {
            oled_off();
        } else {
            render_status_main();
        }
    } else if (is_oled_on()) {
        render_status_secondary();
    }
}

bool process_record_keymap(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        oled_timer = timer_read32();
        add_keylog(keycode);
    }
    return true;
}
