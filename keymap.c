#include QMK_KEYBOARD_H
#include "process_repeat_key.h"

enum layers {
    _BASE,
    _NAV,
    _SYM,
    _NUM,
    _LANG,
    _SYS,
    _MOUSE,
    _QWERTY
};

#define NAV     MO(_NAV)
#define SYM     MO(_SYM)
#define NUM     MO(_NUM)
#define LANG    OSL(_LANG)
#define SYS     MO(_SYS)
#define MOUSE   MO(_MOUSE)
#define MOUSE_TG TG(_MOUSE)
#define VIBRA   PDF(_BASE)
#define QWERTY  PDF(_QWERTY)

// Home-row mods for Vibranium-f home block
#define HRM_S   LALT_T(KC_S)
#define HRM_C   LCTL_T(KC_C)
#define HRM_N   LGUI_T(KC_N)
#define HRM_T   LSFT_T(KC_T)

#define HRM_A   LSFT_T(KC_A)
#define HRM_E   LGUI_T(KC_E)
#define HRM_I   LCTL_T(KC_I)
#define HRM_H   LALT_T(KC_H)

// Shortcuts
#define UNDO    C(KC_Z)
#define REDO    C(S(KC_Z))
#define CUT     C(KC_X)
#define COPY    C(KC_C)
#define PASTE   C(KC_V)
#define SAVE    C(KC_S)
#define FIND    C(KC_F)

#define S_TAB   S(KC_TAB)
#define PREVT   C(S(KC_TAB))
#define NEXTT   C(KC_TAB)
#define ALT_TAB A(KC_TAB)
#define ALT_STAB A(S(KC_TAB))

#define WORD_L  C(KC_LEFT)
#define WORD_R  C(KC_RGHT)
#define SELW_L  C(S(KC_LEFT))
#define SELW_R  C(S(KC_RGHT))
#define C_DEL   C(KC_DEL)
#define C_BSPC  C(KC_BSPC)

#define R_NAV     LT(_NAV, KC_R)  // legacy alias, currently unused
#define REP_NAV   LT(_NAV, KC_F24)

// Thumb and subhome layer-tap keys.
// Chordal Hold + Flow Tap should make these favor taps during same-hand / fast typing.
#define R_NUM LT(_NUM, KC_R)
#define SPC_SYM LT(_SYM, KC_SPC)
#define D_NAV LT(_NAV, KC_D)
#define V_SYM LT(_SYM, KC_V)
#define L_NUM LT(_NUM, KC_L)
#define O_NAV LT(_NAV, KC_O)
#define U_SYM LT(_SYM, KC_U)
#define Y_NUM LT(_NUM, KC_Y)

// Unicode map
enum unicode_names {
    U_AACU, U_AACU_CAP,
    U_EACU, U_EACU_CAP,
    U_IACU, U_IACU_CAP,
    U_OACU, U_OACU_CAP,
    U_UACU, U_UACU_CAP,

    U_AGRV, U_AGRV_CAP,
    U_EGRV, U_EGRV_CAP,
    U_IGRV, U_IGRV_CAP,
    U_OGRV, U_OGRV_CAP,
    U_UGRV, U_UGRV_CAP,

    U_AUML, U_AUML_CAP,
    U_EUML, U_EUML_CAP,
    U_IUML, U_IUML_CAP,
    U_OUML, U_OUML_CAP,
    U_UUML, U_UUML_CAP,

    U_ENYE, U_ENYE_CAP,
    U_CCED, U_CCED_CAP,
    U_ESZ,  U_ESZ_CAP,

    U_IQUE,
    U_IEXC,
    U_EURO,
    U_MDOT
};

const uint32_t PROGMEM unicode_map[] = {
    [U_AACU] = 0x00E1, [U_AACU_CAP] = 0x00C1,
    [U_EACU] = 0x00E9, [U_EACU_CAP] = 0x00C9,
    [U_IACU] = 0x00ED, [U_IACU_CAP] = 0x00CD,
    [U_OACU] = 0x00F3, [U_OACU_CAP] = 0x00D3,
    [U_UACU] = 0x00FA, [U_UACU_CAP] = 0x00DA,

    [U_AGRV] = 0x00E0, [U_AGRV_CAP] = 0x00C0,
    [U_EGRV] = 0x00E8, [U_EGRV_CAP] = 0x00C8,
    [U_IGRV] = 0x00EC, [U_IGRV_CAP] = 0x00CC,
    [U_OGRV] = 0x00F2, [U_OGRV_CAP] = 0x00D2,
    [U_UGRV] = 0x00F9, [U_UGRV_CAP] = 0x00D9,

    [U_AUML] = 0x00E4, [U_AUML_CAP] = 0x00C4,
    [U_EUML] = 0x00EB, [U_EUML_CAP] = 0x00CB,
    [U_IUML] = 0x00EF, [U_IUML_CAP] = 0x00CF,
    [U_OUML] = 0x00F6, [U_OUML_CAP] = 0x00D6,
    [U_UUML] = 0x00FC, [U_UUML_CAP] = 0x00DC,

    [U_ENYE] = 0x00F1, [U_ENYE_CAP] = 0x00D1,
    [U_CCED] = 0x00E7, [U_CCED_CAP] = 0x00C7,
    [U_ESZ]  = 0x00DF, [U_ESZ_CAP]  = 0x1E9E,

    [U_IQUE] = 0x00BF,
    [U_IEXC] = 0x00A1,
    [U_EURO] = 0x20AC,
    [U_MDOT] = 0x00B7
};

// Custom keycodes
enum custom_keycodes {
    HD_TH = SAFE_RANGE,
    HD_CH,
    HD_SH,
    HD_WH,
    HD_GH,
    HD_PH,

    P_DOT_SLSH,
    P_COMM_QUES,
    P_QUOT_DQUO,

    // Legacy/custom punctuation keys referenced in process_record_user().
    // They are not all used in the current keymap, but case labels must be defined.
    P_DOT_COLN,
    P_COMM_SCLN,
    P_MINS_PLUS,
    P_SLSH_QUES,

    BORING,
    CALM,
    OS_MODE,
    HD_MAGIC,
    HD_SKIP_MAGIC,
    SEL_WORD,
    SEL_LINE,
    KILL_LINE
};

// Chordal Hold handedness
// L/R mark true sides; thumbs are exempted with '*'
const char chordal_hold_layout[MATRIX_ROWS][MATRIX_COLS] PROGMEM =
    LAYOUT_split_3x5_3_ex2(
        'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L', 'L',  'R', 'R', 'R', 'R', 'R', 'R',
        'L', 'L', 'L', 'L', 'L',       'R', 'R', 'R', 'R', 'R',
                       '*', '*', '*',  '*', '*', '*'
    );

// Combos
enum combo_events {
    C_TH,
    C_CH,
    C_SH,
    C_WH,
    C_GH,
    C_PH,
    C_Z,
    C_QU
};

const uint16_t PROGMEM th_combo[] = {HRM_T, HRM_N, COMBO_END}; // TN -> TH
const uint16_t PROGMEM ch_combo[] = {HRM_C, HRM_N, COMBO_END}; // CN -> CH
const uint16_t PROGMEM sh_combo[] = {HRM_S, HRM_C, COMBO_END}; // SC -> SH
const uint16_t PROGMEM wh_combo[] = {KC_W,  KC_M,  COMBO_END}; // WM -> WH
const uint16_t PROGMEM gh_combo[] = {KC_G,  KC_M,  COMBO_END}; // GM -> GH
const uint16_t PROGMEM ph_combo[] = {KC_F,  KC_P,  COMBO_END}; // FP -> PH
const uint16_t PROGMEM z_combo[]  = {KC_J,  KC_G,  COMBO_END}; // JG -> Z
const uint16_t PROGMEM qu_combo[] = {HRM_T,  KC_K,  COMBO_END}; // TK -> Q only. Use q + Magic for qu.

combo_t key_combos[] = {
    [C_TH] = COMBO(th_combo, HD_TH),
    [C_CH] = COMBO(ch_combo, HD_CH),
    [C_SH] = COMBO(sh_combo, HD_SH),
    [C_WH] = COMBO(wh_combo, HD_WH),
    [C_GH] = COMBO(gh_combo, HD_GH),
    [C_PH] = COMBO(ph_combo, HD_PH),
    [C_Z]  = COMBO(z_combo, KC_Z),
    [C_QU] = COMBO(qu_combo, KC_Q)
};

uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
    switch (combo_index) {
        case C_TH:
        case C_CH:
        case C_SH:
            return 45;
        case C_WH:
        case C_GH:
        case C_PH:
        case C_Z:
        case C_QU:
            return 55;
        default:
            return COMBO_TERM;
    }
}

bool get_combo_must_tap(uint16_t combo_index, combo_t *combo) {
    return true;  // all smart combos are tap-only now
}

// Startup / wake pulse
#ifdef RGB_MATRIX_ENABLE
static bool     activity_pulse_active = false;
static uint16_t activity_pulse_timer  = 0;
static uint8_t  activity_pulse_hue    = 0;
static uint8_t  activity_pulse_sat    = 0;

static void start_activity_pulse(uint8_t hue, uint8_t sat) {
    rgb_matrix_enable_noeeprom();
    rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
    rgb_matrix_sethsv_noeeprom(hue, sat, 1);
    activity_pulse_hue   = hue;
    activity_pulse_sat   = sat;
    activity_pulse_active = true;
    activity_pulse_timer  = timer_read();
}

void keyboard_post_init_user(void) {
    start_activity_pulse(170, 200); // blue startup pulse
}

void suspend_power_down_user(void) {
    rgb_matrix_disable_noeeprom();
    activity_pulse_active = false;
}

void suspend_wakeup_init_user(void) {
    start_activity_pulse(0, 0); // white wake pulse
}

void housekeeping_task_user(void) {
    if (!activity_pulse_active) {
        return;
    }

    uint16_t t = timer_elapsed(activity_pulse_timer);
    uint8_t v  = 0;

    if (t < 140) {
        v = 8 + (uint8_t)((200 - 8) * t / 140);            // rise
    } else if (t < 1200) {
        v = 200 - (uint8_t)(200 * (t - 140) / 1060);       // fade
    } else {
        rgb_matrix_disable_noeeprom();
        activity_pulse_active = false;
        return;
    }

    rgb_matrix_sethsv_noeeprom(activity_pulse_hue, activity_pulse_sat, v);
}

bool shutdown_user(bool jump_to_bootloader) {
    if (jump_to_bootloader) {
        rgb_matrix_enable_noeeprom();
        rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
        rgb_matrix_sethsv_noeeprom(0, 255, 255); // red
        rgb_matrix_update_pwm_buffers();
    }
    return true;
}
#endif

// Helpers

static bool boring_mode = false;
static bool calm_mode = false;
static bool os_windows_mode = false;

static bool lang_magic_enabled = false; // Disabled by default: avoids conflicts like c+Magic=ch vs c+Magic=ç.

static void tap_code16_without_user_shift(uint16_t keycode) {
    uint8_t mods         = get_mods();
    uint8_t oneshot_mods = get_oneshot_mods();

    // Remove only Shift; preserve Ctrl/Alt/Gui.
    set_mods(mods & ~MOD_MASK_SHIFT);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    send_keyboard_report();

    tap_code16(keycode);

    // Restore held physical mods.
    // Do not restore one-shot Shift; this keypress has consumed it.
    set_mods(mods);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    send_keyboard_report();
}

static inline bool hd_shifted(void) {
    return (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
}

static void send_hd_digraph(uint16_t first, uint16_t second) {
    uint8_t mods         = get_mods();
    uint8_t oneshot_mods = get_oneshot_mods();
    bool shifted         = (mods | oneshot_mods) & MOD_MASK_SHIFT;

    // Remove Shift temporarily, but preserve every other modifier.
    set_mods(mods & ~MOD_MASK_SHIFT);
    send_keyboard_report();
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);

    if (shifted) {
        tap_code16(S(first));   // uppercase only the first letter
    } else {
        tap_code16(first);
    }

    tap_code16(second);         // always unshifted

    // Restore original modifier state.
    set_mods(mods);
    send_keyboard_report();
    set_oneshot_mods(oneshot_mods);
}


static void hd_send_string_unshifted(const char *str) {
    uint8_t mods = get_mods();
    uint8_t oneshot_mods = get_oneshot_mods();
    set_mods(mods & ~MOD_MASK_SHIFT);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    send_keyboard_report();
    send_string(str);
    set_mods(mods);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    send_keyboard_report();
}

// -----------------------------------------------------------------------------
// Magic / Skip Magic history
// -----------------------------------------------------------------------------

static uint16_t hd_magic_last = KC_NO;
static uint16_t hd_magic_prev = KC_NO;
static uint16_t hd_lang_cycle_key = KC_NO;
static uint8_t hd_lang_cycle_step = 0;
static bool hd_lang_cycle_active = false;

#define HD_MAGIC_PENDING_TERM 220

static uint16_t hd_magic_pending = KC_NO;
static uint16_t hd_magic_pending_timer = 0;
static bool     hd_magic_pending_consumed = false;
static uint8_t  hd_magic_last_mods = 0;
static uint8_t  hd_magic_prev_mods = 0;
static uint8_t  hd_magic_pending_mods = 0;

static void hd_magic_clear_history(void) {
    hd_magic_last = KC_NO;
    hd_magic_prev = KC_NO;
    hd_magic_pending = KC_NO;
    hd_magic_pending_consumed = false;
    hd_magic_last_mods = 0;
    hd_magic_prev_mods = 0;
    hd_magic_pending_mods = 0;
    hd_lang_cycle_key = KC_NO;
    hd_lang_cycle_step = 0;
    hd_lang_cycle_active = false;
}

static void hd_magic_set_pending(uint16_t keycode) {
    if (keycode == KC_NO) {
        return;
    }
    hd_magic_pending = keycode;
    hd_magic_pending_timer = timer_read();
    hd_magic_pending_consumed = false;
    hd_magic_pending_mods = get_mods() | get_oneshot_mods();
}

static uint16_t hd_normalize_history_key(uint16_t keycode);

static uint16_t hd_magic_get_trigger(bool *used_pending) {
    if (hd_magic_pending != KC_NO && timer_elapsed(hd_magic_pending_timer) <= HD_MAGIC_PENDING_TERM) {
        if (used_pending) {
            *used_pending = true;
        }
        return hd_magic_pending;
    }

    if (used_pending) {
        *used_pending = false;
    }
    return hd_magic_last;
}

static void hd_magic_consume_pending(void) {
    if (hd_magic_pending != KC_NO) {
        hd_magic_pending_consumed = true;
    }
}

static bool hd_should_suppress_consumed_pending(uint16_t keycode) {
    uint16_t hist = hd_normalize_history_key(keycode);
    if (hd_magic_pending_consumed && hist != KC_NO && hist == hd_magic_pending) {
        hd_magic_pending = KC_NO;
        hd_magic_pending_consumed = false;
        hd_magic_pending_mods = 0;
        return true;
    }
    return false;
}

static bool hd_magic_trigger_shifted(bool used_pending) {
    uint8_t trigger_mods = used_pending ? hd_magic_pending_mods : hd_magic_last_mods;
    return ((trigger_mods | get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) != 0;
}

static void hd_magic_push(uint16_t keycode) {
    if (keycode == KC_NO) {
        return;
    }
    hd_magic_prev = hd_magic_last;
    hd_magic_prev_mods = hd_magic_last_mods;
    hd_magic_last = keycode;
    hd_magic_last_mods = get_mods() | get_oneshot_mods();
    hd_lang_cycle_active = false;
}

static uint16_t hd_normalize_history_key(uint16_t keycode) {
    switch (keycode) {
        // Home-row mod taps.
        case HRM_S: return KC_S;
        case HRM_C: return KC_C;
        case HRM_N: return KC_N;
        case HRM_T: return KC_T;
        case HRM_A: return KC_A;
        case HRM_E: return KC_E;
        case HRM_I: return KC_I;
        case HRM_H: return KC_H;

        // Subhome / thumb layer-tap keys.
        case R_NUM: return KC_R;
        case D_NAV: return KC_D;
        case V_SYM: return KC_V;
        case L_NUM: return KC_L;
        case O_NAV: return KC_O;
        case U_SYM: return KC_U;
        case Y_NUM: return KC_Y;
        case SPC_SYM: return KC_SPC;

        // Letters.
        case KC_A: case KC_B: case KC_C: case KC_D: case KC_E: case KC_F:
        case KC_G: case KC_H: case KC_I: case KC_J: case KC_K: case KC_L:
        case KC_M: case KC_N: case KC_O: case KC_P: case KC_Q: case KC_R:
        case KC_S: case KC_T: case KC_U: case KC_V: case KC_W: case KC_X:
        case KC_Y: case KC_Z:
            return keycode;

        // Digits. Needed for language Magic such as 1 + Magic -> ¡.
        case KC_1: case KC_2: case KC_3: case KC_4: case KC_5:
        case KC_6: case KC_7: case KC_8: case KC_9: case KC_0:
             return keycode;

        // Punctuation and shifted punctuation used for Code Magic.
        case KC_DOT:
        case KC_COMM:
        case KC_MINS:
        case KC_SLSH:
        case KC_QUOT:
        case KC_SCLN:
        case KC_GRV:
        case KC_EQL:
        case KC_BSLS:
        case KC_LBRC:
        case KC_RBRC:
        case KC_SPC:
        case KC_TAB:
        case S(KC_1):     // !
        case S(KC_2):     // @
        case S(KC_3):     // #
        case S(KC_4):     // $
        case S(KC_8):     // *
        case S(KC_9):     // (
        case S(KC_0):     // )
        case S(KC_COMM):  // <
        case S(KC_DOT):   // >
        case S(KC_SCLN):  // :
        case S(KC_GRV):   // ~
        case S(KC_LBRC):  // {
        case S(KC_RBRC):  // }
            return keycode;
    }
    return KC_NO;
}


// -----------------------------------------------------------------------------
// Private Repeat memory
// -----------------------------------------------------------------------------
// We use a private repeat implementation because REP_NAV is a custom dual-role
// key. Calling tap_code16(QK_REP) from a custom key does not reliably invoke
// QMK's built-in Repeat feature. This repeats the last remembered keycode
// directly, which is simpler and more predictable for this keymap.
static uint16_t hd_repeat_last = KC_NO;
static uint8_t  hd_repeat_mods = 0;

#define HD_REPEAT_PENDING_TERM 500
static uint16_t hd_repeat_pending = KC_NO;
static uint8_t  hd_repeat_pending_mods = 0;
static uint16_t hd_repeat_pending_timer = 0;

static void hd_repeat_set_pending(uint16_t keycode) {
    if (keycode == KC_NO) {
        return;
    }
    hd_repeat_pending = keycode;
    hd_repeat_pending_mods = get_mods() | get_oneshot_mods();
    hd_repeat_pending_timer = timer_read();
}

static void hd_repeat_remember(uint16_t keycode) {
    if (keycode == KC_NO) {
        return;
    }

    hd_repeat_last = keycode;
    hd_repeat_mods = get_mods() | get_oneshot_mods();
    hd_repeat_pending = keycode;
    hd_repeat_pending_mods = hd_repeat_mods;
    hd_repeat_pending_timer = timer_read();
}

static bool hd_do_repeat(void) {
    uint16_t repeat_key  = hd_repeat_last;
    uint8_t  repeat_mods = hd_repeat_mods;

    // Use the most recent key press/tap candidate first. This fixes fast
    // sequences where Repeat is pressed before QMK has finished resolving the
    // previous tap-hold key, and it also makes ordinary fast sequences like
    // a b Repeat remember b even if the key is still held.
    if (hd_repeat_pending != KC_NO && timer_elapsed(hd_repeat_pending_timer) <= HD_REPEAT_PENDING_TERM) {
        repeat_key  = hd_repeat_pending;
        repeat_mods = hd_repeat_pending_mods;
        hd_repeat_last = repeat_key;
        hd_repeat_mods = repeat_mods;
    }

    if (repeat_key == KC_NO) {
        return false;
    }

    uint8_t mods         = get_mods();
    uint8_t oneshot_mods = get_oneshot_mods();

    // Approximate QMK Repeat behavior: remembered mods are combined with mods
    // currently held while pressing Repeat.
    set_mods(mods | oneshot_mods | repeat_mods);
    set_oneshot_mods(0);
    send_keyboard_report();

    tap_code16(repeat_key);

    set_mods(mods);
    set_oneshot_mods(oneshot_mods);
    send_keyboard_report();
    return true;
}

static uint16_t hd_repeat_normalize_key(uint16_t keycode) {
    uint16_t hist = hd_normalize_history_key(keycode);
    if (hist != KC_NO) {
        return hist;
    }

    switch (keycode) {
        // Navigation/editing keys that should be repeatable.
        case KC_LEFT: case KC_RGHT: case KC_UP: case KC_DOWN:
        case KC_HOME: case KC_END: case KC_PGUP: case KC_PGDN:
        case KC_BSPC: case KC_DEL: case KC_INS: case KC_TAB: case S_TAB:
        case WORD_L: case WORD_R: case SELW_L: case SELW_R:
        case C_BSPC: case C_DEL:
        case UNDO: case REDO: case CUT: case COPY: case PASTE: case SAVE: case FIND:
        case PREVT: case NEXTT: case ALT_TAB: case ALT_STAB:
        case MS_LEFT: case MS_RGHT: case MS_UP: case MS_DOWN:
        case MS_WHLL: case MS_WHLR: case MS_WHLU: case MS_WHLD:
        case MS_BTN1: case MS_BTN2: case MS_BTN3:
            return keycode;
    }

    return KC_NO;
}

static bool hd_magic_send(uint16_t keycode) {
    if (keycode == KC_NO) {
        return false;
    }
    tap_code16_without_user_shift(keycode);
    hd_magic_push(hd_normalize_history_key(keycode));
    hd_repeat_remember(keycode);
    return true;
}

static bool hd_magic_send_unicode(uint32_t codepoint, uint16_t history_key) {
    register_unicode(codepoint);
    hd_magic_push(history_key);
    return true;
}

static bool hd_send_accented_vowel(uint16_t base, uint8_t step, bool shifted) {
    switch (base) {
        case KC_A:
            return hd_magic_send_unicode(step == 0 ? (shifted ? 0x00C1 : 0x00E1) : step == 1 ? (shifted ? 0x00C0 : 0x00E0) : (shifted ? 0x00C4 : 0x00E4), KC_A);
        case KC_E:
            return hd_magic_send_unicode(step == 0 ? (shifted ? 0x00C9 : 0x00E9) : step == 1 ? (shifted ? 0x00C8 : 0x00E8) : (shifted ? 0x00CB : 0x00EB), KC_E);
        case KC_I:
            return hd_magic_send_unicode(step == 0 ? (shifted ? 0x00CD : 0x00ED) : step == 1 ? (shifted ? 0x00CC : 0x00EC) : (shifted ? 0x00CF : 0x00EF), KC_I);
        case KC_O:
            return hd_magic_send_unicode(step == 0 ? (shifted ? 0x00D3 : 0x00F3) : step == 1 ? (shifted ? 0x00D2 : 0x00F2) : (shifted ? 0x00D6 : 0x00F6), KC_O);
        case KC_U:
            return hd_magic_send_unicode(step == 0 ? (shifted ? 0x00DA : 0x00FA) : step == 1 ? (shifted ? 0x00D9 : 0x00F9) : (shifted ? 0x00DC : 0x00FC), KC_U);
    }
    return false;
}

static bool hd_do_lang_magic(void) {
    bool shifted = hd_shifted();

    switch (hd_magic_last) {
        case KC_N: return hd_magic_send_unicode(shifted ? 0x00D1 : 0x00F1, KC_N); // ñ/Ñ
        case KC_C: return hd_magic_send_unicode(shifted ? 0x00C7 : 0x00E7, KC_C); // ç/Ç
        case KC_S: return hd_magic_send_unicode(shifted ? 0x1E9E : 0x00DF, KC_S); // ß/ẞ
        case KC_1: return hd_magic_send_unicode(0x00A1, KC_1);                  // ¡
        case KC_SLSH: return hd_magic_send_unicode(0x00BF, KC_SLSH);            // ¿
        case KC_DOT: return hd_magic_send_unicode(0x00B7, KC_DOT);              // ·
    }

    switch (hd_magic_last) {
        case KC_A: case KC_E: case KC_I: case KC_O: case KC_U: {
            uint16_t base = hd_magic_last;
            if (hd_lang_cycle_active && hd_lang_cycle_key == base) {
                tap_code(KC_BSPC);
                hd_lang_cycle_step = (hd_lang_cycle_step + 1) % 3;
            } else {
                hd_lang_cycle_key = base;
                hd_lang_cycle_step = 0;
            }

            bool ok = hd_send_accented_vowel(base, hd_lang_cycle_step, shifted);
            if (ok) {
                // hd_magic_push() resets the cycle flag, so restore it after sending.
                hd_lang_cycle_key = base;
                hd_lang_cycle_active = true;
            }
            return ok;
        }
    }

    return false;
}


// -----------------------------------------------------------------------------
// Direct Language Magic on the normal Magic key
// -----------------------------------------------------------------------------
// Conflict policy:
// - c/s keep ch/sh, so ç/ß move to x/z.
// - n keeps n't, so ñ moves to y.
// - i + Magic becomes "I" only in standalone-I context; otherwise it becomes í/ì/ï.
// - Vowels cycle acute -> grave -> umlaut on repeated Magic.

static uint32_t hd_accented_vowel_codepoint(uint16_t base, uint8_t step, bool shifted) {
    switch (base) {
        case KC_A:
            return step == 0 ? (shifted ? 0x00C1 : 0x00E1) : step == 1 ? (shifted ? 0x00C0 : 0x00E0) : (shifted ? 0x00C4 : 0x00E4);
        case KC_E:
            return step == 0 ? (shifted ? 0x00C9 : 0x00E9) : step == 1 ? (shifted ? 0x00C8 : 0x00E8) : (shifted ? 0x00CB : 0x00EB);
        case KC_I:
            return step == 0 ? (shifted ? 0x00CD : 0x00ED) : step == 1 ? (shifted ? 0x00CC : 0x00EC) : (shifted ? 0x00CF : 0x00EF);
        case KC_O:
            return step == 0 ? (shifted ? 0x00D3 : 0x00F3) : step == 1 ? (shifted ? 0x00D2 : 0x00F2) : (shifted ? 0x00D6 : 0x00F6);
        case KC_U:
            return step == 0 ? (shifted ? 0x00DA : 0x00FA) : step == 1 ? (shifted ? 0x00D9 : 0x00F9) : (shifted ? 0x00DC : 0x00FC);
    }
    return 0;
}

static bool hd_i_magic_is_pronoun_context(bool used_pending) {
    // Keep i + Magic dedicated to accented vowels for now. The English "I"
    // brief was useful in theory, but multilingual typing benefits more from
    // making í/ì/ï deterministic and conflict-free.
    (void)used_pending;
    return false;
}

static bool hd_magic_send_unicode_replace_or_pending(uint32_t codepoint, uint16_t history_key, bool used_pending) {
    if (codepoint == 0) {
        return false;
    }

    // Direct language Magic is replacement-style: x+Magic should become ç,
    // y+Magic should become ñ, etc. In this QMK/tap-hold timing, even a
    // "pending" tap-hold letter is usually already emitted by the time Magic
    // runs, so always erase the plain trigger before sending Unicode.
    tap_code(KC_BSPC);

    register_unicode(codepoint);
    hd_magic_push(history_key);
    hd_repeat_remember(history_key);

    if (used_pending) {
        hd_magic_consume_pending();
    }

    return true;
}

static bool hd_magic_replace_with_key(uint16_t keycode, uint16_t history_key, bool used_pending) {
    tap_code(KC_BSPC);
    tap_code16_without_user_shift(keycode);
    hd_magic_push(history_key);
    hd_repeat_remember(keycode);

    if (used_pending) {
        hd_magic_consume_pending();
    }

    return true;
}

static bool hd_magic_vowel(uint16_t base, bool used_pending) {
    if (base != KC_A && base != KC_E && base != KC_I && base != KC_O && base != KC_U) {
        return false;
    }

    if (base == KC_I && hd_i_magic_is_pronoun_context(used_pending)) {
        // i + Magic -> I in English-pronoun context. Replacement-style:
        // erase the emitted i, then send I.
        tap_code(KC_BSPC);
        tap_code16_without_user_shift(S(KC_I));
        hd_magic_push(KC_I);
        hd_repeat_remember(S(KC_I));
        if (used_pending) {
            hd_magic_consume_pending();
        }
        return true;
    }

    if (hd_lang_cycle_active && hd_lang_cycle_key == base) {
        // Repeated Magic cycles the accent and replaces the previous accented vowel.
        tap_code(KC_BSPC);
        hd_lang_cycle_step = (hd_lang_cycle_step + 1) % 3;
    } else {
        hd_lang_cycle_key = base;
        hd_lang_cycle_step = 0;
        // First Magic replaces the already-emitted plain vowel. This also
        // applies to pending HRM/layer-tap vowels because QMK has usually
        // emitted the tap before Magic runs.
        tap_code(KC_BSPC);
    }

    uint32_t cp = hd_accented_vowel_codepoint(base, hd_lang_cycle_step, hd_magic_trigger_shifted(used_pending));
    register_unicode(cp);

    hd_magic_push(base);
    hd_lang_cycle_key = base;
    hd_lang_cycle_active = true;
    hd_repeat_remember(base);

    if (used_pending) {
        hd_magic_consume_pending();
    }

    return true;
}

static bool hd_do_direct_language_magic(uint16_t trigger, bool used_pending) {
    switch (trigger) {
        case KC_A:
        case KC_E:
        case KC_I:
        case KC_O:
        case KC_U:
            return hd_magic_vowel(trigger, used_pending);

        case KC_K:
            return hd_magic_replace_with_key(hd_magic_trigger_shifted(used_pending) ? S(KC_Q) : KC_Q, KC_Q, used_pending); // k + Magic -> q/Q

        case KC_Y:
            return hd_magic_send_unicode_replace_or_pending(hd_magic_trigger_shifted(used_pending) ? 0x00D1 : 0x00F1, KC_N, used_pending); // y + Magic -> ñ/Ñ
        case KC_X:
            return hd_magic_send_unicode_replace_or_pending(hd_magic_trigger_shifted(used_pending) ? 0x00C7 : 0x00E7, KC_C, used_pending); // x + Magic -> ç/Ç
        case KC_Z:
            return hd_magic_send_unicode_replace_or_pending(hd_magic_trigger_shifted(used_pending) ? 0x1E9E : 0x00DF, KC_S, used_pending); // z + Magic -> ß/ẞ

        case KC_1:
            return hd_magic_send_unicode_replace_or_pending(0x00A1, KC_1, used_pending);       // 1 + Magic -> ¡
        case S(KC_SLSH):
            return hd_magic_send_unicode_replace_or_pending(0x00BF, KC_SLSH, used_pending);    // ? + Magic -> ¿
        case S(KC_2):
            return hd_magic_send_unicode_replace_or_pending(0x20AC, S(KC_2), used_pending);    // @ + Magic -> €
        case KC_SCLN:
            return hd_magic_send_unicode_replace_or_pending(0x00B7, KC_DOT, used_pending);     // ; + Magic -> ·
    }

    return false;
}

static bool hd_do_base_magic(void) {
    if (boring_mode) {
        return false;
    }

    bool used_pending = false;
    uint16_t trigger = hd_magic_get_trigger(&used_pending);
    if (trigger == KC_NO) {
        return false;
    }

    // Language Magic is disabled by default because normal Magic already uses
    // c/s/n/vowels for digraphs and briefs. Keep it behind an explicit flag for later.
    if (lang_magic_enabled && IS_LAYER_ON(_LANG)) {
        return hd_do_lang_magic();
    }

    // Direct language Magic lives on the normal Magic key and avoids conflicts
    // by using proxy triggers where normal Magic already has a high-value rule.
    // Keep c/s/n for ch/sh/n't; use x/z/y for ç/ß/ñ.
    if (hd_do_direct_language_magic(trigger, used_pending)) {
        return true;
    }

    // Pending Magic uses the still-held tap-hold key only as a trigger.
    // In this keymap/QMK timing, the base tap is often already emitted by the
    // time Magic runs. Therefore pending Magic must send only the completion,
    // not the whole digraph/snippet, or fast t+Magic becomes tth.
    if (used_pending) {
        switch (trigger) {
            case KC_T:
            case KC_C:
            case KC_S:
            case KC_W:
            case KC_G:
            case KC_P:
                hd_magic_send(KC_H);      // pending t/c/s/w/g/p + Magic -> add h
                hd_magic_consume_pending();
                return true;
            case KC_I:
                // pending i + Magic -> I. The base i has usually already been
                // emitted, so replace it just like the non-pending path.
                tap_code(KC_BSPC);
                tap_code16(S(KC_I));
                hd_magic_push(KC_I);
                hd_repeat_remember(S(KC_I));
                hd_magic_consume_pending();
                return true;
            case KC_N:
                hd_send_string_unshifted("'t");       // pending n + Magic -> n't
                hd_magic_push(KC_T);
                hd_repeat_remember(KC_T);
                hd_magic_consume_pending();
                return true;
            case KC_H:
                hd_send_string_unshifted("ave");      // pending h + Magic -> have
                hd_magic_push(KC_E);
                hd_repeat_remember(KC_E);
                hd_magic_consume_pending();
                return true;
            case KC_SPC:
                hd_send_string_unshifted("the");      // pending Space + Magic -> " the"
                hd_magic_push(KC_E);
                hd_repeat_remember(KC_E);
                hd_magic_consume_pending();
                return true;
        }
    }

    // Non-pending Magic: the trigger key has already been emitted, so append
    // only the completion or replace the already-emitted trigger where needed.
    switch (trigger) {
        case KC_Q:
            return hd_magic_send(KC_U); // q + Magic -> qu
        case KC_T:
        case KC_C:
        case KC_S:
        case KC_W:
        case KC_G:
        case KC_P:
            return hd_magic_send(KC_H); // H-digraph Magic
    }

    if (calm_mode) {
        return false;
    }

    // Tiny briefs. These are intentionally small and non-editor-specific.
    switch (trigger) {
        case KC_I:
            // i + Magic -> I.
            tap_code(KC_BSPC);
            tap_code16(S(KC_I));
            hd_magic_push(KC_I);
            hd_repeat_remember(S(KC_I));
            return true;
        case KC_N:
            hd_send_string_unshifted("'t");      // n + Magic -> n't
            hd_magic_push(KC_T);
            hd_repeat_remember(KC_T);
            return true;
        case KC_H:
            hd_send_string_unshifted("ave");     // h + Magic -> have
            hd_magic_push(KC_E);
            hd_repeat_remember(KC_E);
            return true;
    }

    // Code / punctuation Magic.
    switch (trigger) {
        case KC_DOT:      return hd_magic_send(KC_SLSH);     // ./
        case KC_MINS:     return hd_magic_send(S(KC_DOT));   // ->
        case KC_SLSH:     return hd_magic_send(S(KC_8));     // /*
        case S(KC_8):     return hd_magic_send(KC_SLSH);     // */
        case KC_COMM:     return hd_magic_send(KC_SPC);      // comma-space
        case S(KC_SCLN):  return hd_magic_send(KC_SPC);      // colon-space
        case S(KC_1):     return hd_magic_send(KC_EQL);      // !=
        case S(KC_COMM):  return hd_magic_send(KC_EQL);      // <=
        case S(KC_DOT):   return hd_magic_send(KC_EQL);      // >=
        case S(KC_GRV):   return hd_magic_send(KC_SLSH);     // ~/
        case S(KC_4):
            tap_code16_without_user_shift(S(KC_LBRC));       // ${}
            tap_code16_without_user_shift(S(KC_RBRC));
            tap_code(KC_LEFT);
            hd_magic_push(S(KC_RBRC));
            hd_repeat_remember(S(KC_RBRC));
            return true;
        case KC_SPC:
            hd_send_string_unshifted("the");                // minimal brief: Space + Magic -> the
            hd_magic_push(KC_E);
            hd_repeat_remember(KC_E);
            return true;
    }

    return false;
}

static bool hd_do_skip_magic(void) {
    if (boring_mode || calm_mode) {
        return false;
    }

    switch (hd_magic_prev) {
        case KC_O: return hd_magic_send(KC_E);
        case KC_I: return hd_magic_send(KC_Y);
        case KC_N: return hd_magic_send(KC_L);
        case KC_T: return hd_magic_send(KC_D);
        case KC_C: return hd_magic_send(KC_P);
    }

    // Skip Repeat fallback: key1 key2 SkipMagic -> key1 key2 key1.
    return hd_magic_send(hd_magic_prev);
}

// -----------------------------------------------------------------------------
// Repeat / Alt Repeat tuning
// -----------------------------------------------------------------------------

uint16_t get_alt_repeat_key_keycode_user(uint16_t keycode, uint8_t mods) {
    bool shifted = mods & MOD_MASK_SHIFT;

    if ((mods & MOD_MASK_CTRL) && keycode == KC_TAB) {
        return shifted ? C(KC_TAB) : C(S(KC_TAB));
    }

    if ((mods & MOD_MASK_ALT) && keycode == KC_TAB) {
        return shifted ? A(KC_TAB) : A(S(KC_TAB));
    }

    if ((mods & MOD_MASK_CTRL) && keycode == KC_Z) {
        return C(S(KC_Z));
    }

    return KC_TRNS;
}

bool remember_last_key_user(uint16_t keycode, keyrecord_t *record, uint8_t *remembered_mods) {
    switch (keycode) {
        case BORING:
        case CALM:
        case OS_MODE:
        case REP_NAV:
        case HD_MAGIC:
        case HD_SKIP_MAGIC:
        case QK_LLCK:
        case QK_BOOT:
            return false;
    }
    return true;
}

static bool hd_rep_nav_repeat_sent = false;

// -----------------------------------------------------------------------------
// Main key processing
// -----------------------------------------------------------------------------

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case BORING:
            if (record->event.pressed) {
                boring_mode = !boring_mode;
                hd_magic_clear_history();
            }
            return false;

        case CALM:
            if (record->event.pressed) {
                calm_mode = !calm_mode;
                hd_magic_clear_history();
            }
            return false;

        case OS_MODE:
            if (record->event.pressed) {
                os_windows_mode = !os_windows_mode;
            }
            return false;

        case REP_NAV:
            // REP_NAV = LT(_NAV, KC_F24). Let QMK handle the hold side normally.
            // Some QMK paths expose the tap as REP_NAV on release; others expose
            // the tap key KC_F24. Handle both, guarded against double-repeat.
            if (record->event.pressed) {
                hd_rep_nav_repeat_sent = false;
                return true;
            }
            if (record->tap.count && !hd_rep_nav_repeat_sent) {
                hd_do_repeat();
                hd_rep_nav_repeat_sent = true;
                return false;
            }
            return true;

        case KC_F24:
            // Dummy tap side of REP_NAV. Never send F24 to the host.
            if (record->event.pressed && !hd_rep_nav_repeat_sent) {
                hd_do_repeat();
                hd_rep_nav_repeat_sent = true;
            }
            return false;

        case HD_MAGIC:
            if (record->event.pressed) {
                hd_do_base_magic();
            }
            return false;

        case HD_SKIP_MAGIC:
            if (record->event.pressed) {
                hd_do_skip_magic();
            }
            return false;

        case SEL_WORD:
            if (record->event.pressed) {
                tap_code16(SELW_R);
            }
            return false;

        case SEL_LINE:
            if (record->event.pressed) {
                tap_code(KC_HOME);
                tap_code16(S(KC_END));
            }
            return false;

        case KILL_LINE:
            if (record->event.pressed) {
                tap_code16(S(KC_END));
                tap_code(KC_DEL);
            }
            return false;

        case HD_TH:
            if (record->event.pressed) { send_hd_digraph(KC_T, KC_H); hd_magic_push(KC_H); hd_repeat_remember(KC_H); }
            return false;
        case HD_CH:
            if (record->event.pressed) { send_hd_digraph(KC_C, KC_H); hd_magic_push(KC_H); hd_repeat_remember(KC_H); }
            return false;
        case HD_SH:
            if (record->event.pressed) { send_hd_digraph(KC_S, KC_H); hd_magic_push(KC_H); hd_repeat_remember(KC_H); }
            return false;
        case HD_WH:
            if (record->event.pressed) { send_hd_digraph(KC_W, KC_H); hd_magic_push(KC_H); hd_repeat_remember(KC_H); }
            return false;
        case HD_GH:
            if (record->event.pressed) { send_hd_digraph(KC_G, KC_H); hd_magic_push(KC_H); hd_repeat_remember(KC_H); }
            return false;
        case HD_PH:
            if (record->event.pressed) { send_hd_digraph(KC_P, KC_H); hd_magic_push(KC_H); hd_repeat_remember(KC_H); }
            return false;
        case KC_ESC:
        case KC_ENT:
            if (record->event.pressed) {
                hd_magic_clear_history();
            }
            break;

        case P_DOT_COLN:
            if (record->event.pressed) {
                uint16_t out = hd_shifted() ? S(KC_SCLN) : KC_DOT;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? S(KC_SCLN) : KC_DOT);
                hd_repeat_remember(out);
            }
            return false;

        case P_DOT_SLSH:
            if (record->event.pressed) {
                uint16_t out = hd_shifted() ? KC_SLSH : KC_DOT;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? KC_SLSH : KC_DOT);
                hd_repeat_remember(out);
            }
            return false;

        case P_COMM_SCLN:
            if (record->event.pressed) {
                uint16_t out = hd_shifted() ? KC_SCLN : KC_COMM;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? KC_SCLN : KC_COMM);
                hd_repeat_remember(out);
            }
            return false;

        case P_COMM_QUES:
            if (record->event.pressed) {
                uint16_t out = hd_shifted() ? S(KC_SLSH) : KC_COMM;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? S(KC_SLSH) : KC_COMM);
                hd_repeat_remember(out);
            }
            return false;

        case P_MINS_PLUS:
            if (record->event.pressed) {
                uint16_t out = hd_shifted() ? S(KC_EQL) : KC_MINS;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? S(KC_EQL) : KC_MINS);
                hd_repeat_remember(out);
            }
            return false;

        case P_SLSH_QUES:
            if (record->event.pressed) {
                uint16_t out = hd_shifted() ? S(KC_SLSH) : KC_SLSH;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? S(KC_SLSH) : KC_SLSH);
                hd_repeat_remember(out);
            }
            return false;

        case P_QUOT_DQUO:
            if (record->event.pressed) {
                uint16_t out = hd_shifted() ? S(KC_QUOT) : KC_QUOT;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? S(KC_QUOT) : KC_QUOT);
                hd_repeat_remember(out);
            }
            return false;
    }

    // Track normal taps for Magic / SkipMagic and private Repeat
    if (record->event.pressed) {
        uint16_t hist = hd_normalize_history_key(keycode);

        uint16_t rep  = hd_repeat_normalize_key(keycode);

        switch (keycode) {
            case HRM_S: case HRM_C: case HRM_N: case HRM_T:
            case HRM_A: case HRM_E: case HRM_I: case HRM_H:
            case R_NUM: case D_NAV: case V_SYM: case L_NUM:
            case O_NAV: case U_SYM: case Y_NUM: case SPC_SYM:
                if (hist != KC_NO) {
                    hd_magic_set_pending(hist);
                    hd_repeat_set_pending(hist);
                }
                break; // tap-hold keys are handled on release if tapped.
            default:
                if (hist != KC_NO) {
                    hd_magic_push(hist);
                }
                if (rep != KC_NO) {
                    hd_repeat_set_pending(rep);
                    hd_repeat_remember(rep);
                }
                break;
        }
    } else {
        if (record->tap.count > 0) {
            uint16_t hist = hd_normalize_history_key(keycode);
            if (hist != KC_NO) {
                if (hd_should_suppress_consumed_pending(keycode)) {
                    return false;
                }
                hd_magic_push(hist);
                hd_repeat_remember(hist);
            }
        }
    }

    return true;
}
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /*
     * Layer 0 — Vibranium-f + Vim/terminal-oriented base
     *
     *
     * X W M G J | Lang      ||    Enter Esc ./ ,? '" Tab
     * S C N T K | Sys       ||    Sys Magic A E I H
     * F P L/Num D/Nav V/Sym ||    SkipMagic U/Sym O/Nav Y/Num B
     * CapsWord Boring R/Num ||    Repeat/Nav  Space/Sym  Backspace
     * Smart behavior:
     * - JG -> Z
     * - TK -> QU
     * - Hold TK long enough -> Q
     * - TN/CN/SC/WM/GM/FP -> TH/CH/SH/WH/GH/PH
     */
    [_BASE] = LAYOUT_split_3x5_3_ex2(

        KC_X,    KC_W,    KC_M,    KC_G,    KC_J,    LANG,       KC_TAB,  KC_ENT,         KC_ESC,   P_DOT_SLSH,  P_COMM_QUES, P_QUOT_DQUO,
        HRM_S,   HRM_C,   HRM_N,   HRM_T,   KC_K,    SYS,        SYS,     HD_MAGIC,       HRM_A,    HRM_E,       HRM_I,       HRM_H,
        KC_F,    KC_P,    L_NUM,   D_NAV,   V_SYM,                        /*HD_SKIP_MAGIC*/ KC_R,  U_SYM,    O_NAV,       Y_NUM,       KC_B,
                                   CW_TOGG, BORING, R_NUM,        REP_NAV, SPC_SYM, KC_BSPC
    ),

    /*
     * Layer 1 — Nav/Edit
     */
    [_NAV] = LAYOUT_split_3x5_3_ex2(
        UNDO,    CUT,     COPY,    PASTE,   SAVE,    _______,     _______, KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_DEL,
        KC_LALT, KC_LCTL, KC_LGUI,  KC_LSFT, KC_TAB,  _______,     _______, KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_BSPC,
        FIND,    S_TAB,   PREVT,   NEXTT,   ALT_TAB,                         KC_INS,  WORD_L,  WORD_R,  C_BSPC,  C_DEL,
                                    _______, QK_LLCK, QK_LLCK,   QK_AREP, QK_LLCK, _______
    ),

    /*
     * Layer 2 — Symbols
     *
     * !   @   #   $   %      _____     _____     XXXXX  q  |   \   z
     * ^   &   *   +   =      _____     _____     XXXXX  (  {   [   <
     * ~   -   _   :   ;                          `      )  }   ]   >
     *
     *            _____ _____ Num        _____ _____ _____
     */
    [_SYM] = LAYOUT_split_3x5_3_ex2(
        S(KC_1),   S(KC_2),   S(KC_3),   S(KC_4),   S(KC_5),   _______,     _______, XXXXXXX,    KC_Q,    S(KC_BSLS), KC_BSLS,    KC_Z,
        S(KC_6),   S(KC_7),   S(KC_8),   S(KC_EQL), KC_EQL,    _______,     _______, KC_MINS,    S(KC_9),    S(KC_LBRC), KC_LBRC,    S(KC_COMM),
        S(KC_GRV), KC_MINS,   S(KC_MINS), S(KC_SCLN), KC_SCLN,                       KC_GRV,     S(KC_0),    S(KC_RBRC), KC_RBRC,    S(KC_DOT),
                                          _______, QK_LLCK, QK_LLCK,                      QK_LLCK, QK_LLCK, _______
    ),

    /*
     * Layer 3 — Numbers/Fn
     */
    [_NUM] = LAYOUT_split_3x5_3_ex2(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,     _______, KC_SLSH, KC_7,    KC_8,    KC_9,    KC_MINS,
        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,     _______, S(KC_8), KC_4,    KC_5,    KC_6,    S(KC_EQL),
        KC_F11,  KC_F12,  KC_0,  KC_DOT,  KC_EQL,                            KC_DOT, KC_1,    KC_2,    KC_3,    KC_0,
                                      _______, QK_LLCK, QK_LLCK,     QK_LLCK, QK_LLCK, _______
    ),

    /*
     * Layer 4 — Language / Unicode
     */
    [_LANG] = LAYOUT_split_3x5_3_ex2(
        _______, _______, UP(U_ENYE, U_ENYE_CAP), UP(U_CCED, U_CCED_CAP), UP(U_ESZ, U_ESZ_CAP), _______,           _______, UP(U_AACU, U_AACU_CAP), UP(U_EACU, U_EACU_CAP), UP(U_IACU, U_IACU_CAP), UP(U_OACU, U_OACU_CAP), UP(U_UACU, U_UACU_CAP),

        _______, _______, UM(U_IQUE), UM(U_MDOT), UM(U_EURO), _______,                                             _______, UP(U_AGRV, U_AGRV_CAP), UP(U_EGRV, U_EGRV_CAP), UP(U_IGRV, U_IGRV_CAP), UP(U_OGRV, U_OGRV_CAP), UP(U_UGRV, U_UGRV_CAP),

        _______, _______, UM(U_IEXC), _______, _______,                                                                     UP(U_AUML, U_AUML_CAP), UP(U_EUML, U_EUML_CAP), UP(U_IUML, U_IUML_CAP), UP(U_OUML, U_OUML_CAP), UP(U_UUML, U_UUML_CAP),
                                      _______, _______, _______,     _______, _______, _______
    ),

    /*
     * Layer 5 — System
     */
    [_SYS] = LAYOUT_split_3x5_3_ex2(
        UC_LINX, UC_WIN,  VIBRA,   QWERTY,  MOUSE_TG, QK_BOOT,    QK_BOOT, KC_PSCR, KC_SCRL, KC_PAUS, XXXXXXX, XXXXXXX,
        KC_MUTE, KC_VOLD, KC_VOLU, KC_BRID, KC_BRIU, _______,     _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_MSTP, XXXXXXX,
        CM_TOGG, CALM,    OS_MODE, KC_NUM,  KC_APP,                         SH_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                      _______, _______, _______,     _______, _______, _______
    ),

    /*
     * Layer 6 — Mouse / scroll
     */
    [_MOUSE] = LAYOUT_split_3x5_3_ex2(
        _______, _______, _______, _______, MOUSE_TG, _______,      _______, MS_WHLL, MS_WHLU, MS_WHLR, _______, _______,
        _______, _______, _______, _______, _______, _______,      _______, MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, MS_BTN2,
        _______, _______, _______, _______, _______,               _______, MS_BTN1, MS_WHLD, MS_BTN3, _______,
                                      _______, QK_LLCK, _______,  _______, _______, _______
    ),
    /*
     * Layer 7 — QWERTY rescue
     */
    [_QWERTY] = LAYOUT_split_3x5_3_ex2(
        KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    LANG,        NAV,     KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,
        KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    SYS,         SYS,     KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN,
        KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                            KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH,
                                      CW_TOGG,     KC_SPC,  NAV,     SYM,    KC_SPC,  KC_BSPC
    )
};
