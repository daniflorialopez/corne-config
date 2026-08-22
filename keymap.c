#include QMK_KEYBOARD_H
#include "process_repeat_key.h"

// Default layers (_BASE, _QWERTY) must stay below all momentary layers:
// the default layer is looked up at its bit position, so a high default
// layer shadows every layer beneath it.
enum layers {
    _BASE,
    _QWERTY,
    _NAV,
    _SYM,
    _NUM,
    _LANG,
    _SYS,
    _MOUSE
};

#define NAV     MO(_NAV)
#define SYM     MO(_SYM)
#define LANG    OSL(_LANG)
#define SYS     MO(_SYS)
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

#define REP_NAV   LT(_NAV, KC_F24)

// Layer-tap keys. Only the thumbs and one right-hand subhome key remain:
// every other subhome layer-tap was removed because a mistimed hold on a
// letter key is the misfire that hurts most.
#define R_NUM   LT(_NUM, KC_R)    // left inner thumb
#define SPC_SYM LT(_SYM, KC_SPC)  // right middle thumb
#define O_NUM   LT(_NUM, KC_O)    // right-hand subhome access to _NUM

// -----------------------------------------------------------------------------
// Compose sequences
// -----------------------------------------------------------------------------
// Accented characters used to go out as raw Unicode codepoints under
// UNICODE_MODE_LINUX, which types Ctrl+Shift+U, then the hex digits, then
// Enter -- and relies on an IBus daemon to swallow that and hand back one
// character. There is no IBus on this machine: Wayland under Hyprland with
// GTK_IM_MODULE/QT_IM_MODULE/XMODIFIERS all unset. So nothing intercepted the
// chord. A terminal drops Shift when computing the control code, saw a bare
// Ctrl+U, and ran readline's unix-line-discard -- hence the disappearing
// lines -- and then the hex digits arrived as literal text.
//
// The Compose key is resolved by libxkbcommon inside each client, so it needs
// no daemon and behaves the same in a terminal, a browser and an Electron app.
// It is also three keycodes instead of ten, which closes the race between the
// backspace and the character meant to replace it.
//
// REQUIRES `compose:menu` in the xkb options, which maps the Menu key to
// Multi_key. Under home-manager:
//
//     wayland.windowManager.hyprland.settings.input.kb_options = "compose:menu";
//
// Every sequence below was taken from the system Compose file and assumes a
// `us` base layout.
#define COMPOSE KC_APP

// How the uppercase form of a sequence is spelled.
enum hd_compose_case {
    HD_CASE_NONE,    // no uppercase form -- punctuation and symbols
    HD_CASE_SECOND,  // shift the second key:  Compose ' A  -> A-acute
    HD_CASE_BOTH     // shift both keys:       Compose S S  -> capital sharp s
};

typedef struct {
    uint16_t first;
    uint16_t second;
    uint8_t  cased;
} hd_compose_seq_t;

// Custom keycodes
enum custom_keycodes {
    P_DOT_SLSH = SAFE_RANGE,
    P_COMM_QUES,
    P_QUOT_DQUO,

    HD_MAGIC,
    HD_ARCANE,
    SEL_WORD,
    SEL_LINE,
    KILL_LINE,

    // Compose characters, used by the Language layer. The accented vowels must
    // stay as three blocks of five in a-e-i-o-u order: hd_accented_vowel_seq()
    // indexes them arithmetically rather than switching on every combination.
    XC_AACU, XC_EACU, XC_IACU, XC_OACU, XC_UACU,
    XC_AGRV, XC_EGRV, XC_IGRV, XC_OGRV, XC_UGRV,
    XC_AUML, XC_EUML, XC_IUML, XC_OUML, XC_UUML,

    XC_ENYE, XC_CCED, XC_ESZ,
    XC_IEXC, XC_IQUE, XC_EURO, XC_MDOT,
    XC_LAST
};

#define XC_FIRST  XC_AACU
#define HD_SEQ(xc) (&hd_compose_table[(xc) - XC_FIRST])

static const hd_compose_seq_t hd_compose_table[XC_LAST - XC_FIRST] = {
    [XC_AACU - XC_FIRST] = {KC_QUOT,    KC_A,       HD_CASE_SECOND},  // á Á   Compose ' a
    [XC_EACU - XC_FIRST] = {KC_QUOT,    KC_E,       HD_CASE_SECOND},  // é É
    [XC_IACU - XC_FIRST] = {KC_QUOT,    KC_I,       HD_CASE_SECOND},  // í Í
    [XC_OACU - XC_FIRST] = {KC_QUOT,    KC_O,       HD_CASE_SECOND},  // ó Ó
    [XC_UACU - XC_FIRST] = {KC_QUOT,    KC_U,       HD_CASE_SECOND},  // ú Ú

    [XC_AGRV - XC_FIRST] = {KC_GRV,     KC_A,       HD_CASE_SECOND},  // à À   Compose ` a
    [XC_EGRV - XC_FIRST] = {KC_GRV,     KC_E,       HD_CASE_SECOND},  // è È
    [XC_IGRV - XC_FIRST] = {KC_GRV,     KC_I,       HD_CASE_SECOND},  // ì Ì
    [XC_OGRV - XC_FIRST] = {KC_GRV,     KC_O,       HD_CASE_SECOND},  // ò Ò
    [XC_UGRV - XC_FIRST] = {KC_GRV,     KC_U,       HD_CASE_SECOND},  // ù Ù

    [XC_AUML - XC_FIRST] = {S(KC_QUOT), KC_A,       HD_CASE_SECOND},  // ä Ä   Compose " a
    [XC_EUML - XC_FIRST] = {S(KC_QUOT), KC_E,       HD_CASE_SECOND},  // ë Ë
    [XC_IUML - XC_FIRST] = {S(KC_QUOT), KC_I,       HD_CASE_SECOND},  // ï Ï
    [XC_OUML - XC_FIRST] = {S(KC_QUOT), KC_O,       HD_CASE_SECOND},  // ö Ö
    [XC_UUML - XC_FIRST] = {S(KC_QUOT), KC_U,       HD_CASE_SECOND},  // ü Ü

    [XC_ENYE - XC_FIRST] = {S(KC_GRV),  KC_N,       HD_CASE_SECOND},  // ñ Ñ   Compose ~ n
    [XC_CCED - XC_FIRST] = {KC_COMM,    KC_C,       HD_CASE_SECOND},  // ç Ç   Compose , c
    [XC_ESZ  - XC_FIRST] = {KC_S,       KC_S,       HD_CASE_BOTH},    // ß ẞ   Compose s s
    [XC_IEXC - XC_FIRST] = {S(KC_1),    S(KC_1),    HD_CASE_NONE},    // ¡     Compose ! !
    [XC_IQUE - XC_FIRST] = {S(KC_SLSH), S(KC_SLSH), HD_CASE_NONE},    // ¿     Compose ? ?
    [XC_EURO - XC_FIRST] = {KC_C,       KC_EQL,     HD_CASE_NONE},    // €     Compose c =
    [XC_MDOT - XC_FIRST] = {KC_DOT,     KC_MINS,    HD_CASE_NONE}     // ·     Compose . -
};

_Static_assert(XC_AGRV - XC_AACU == 5 && XC_AUML - XC_AACU == 10,
               "hd_accented_vowel_seq() indexes the accent blocks arithmetically");

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
//
// The old h-digraph combos (TN/CN/SC/WM/GM/FP) are gone. They were horizontal
// chords on adjacent fingers, which a normal letter roll can trigger by
// accident; that is the misfire class this keymap no longer tolerates.
//
// What replaces them is a family of SAME-FINGER VERTICAL combos on the right
// hand. One finger cannot roll vertically at typing speed, so no ordinary
// letter sequence can produce them. Each one is an ALTERNATE home for a
// left-hand letter that is expensive to reach, used at will when the left
// hand is busy -- never a replacement for the base key.
enum combo_events {
    C_Z,
    C_QU,
    C_X,
    C_F,
    C_K,
    C_V,
    C_J,
    C_S
};

const uint16_t PROGMEM z_combo[]  = {KC_J,  KC_G, COMBO_END}; // JG -> Z
const uint16_t PROGMEM qu_combo[] = {HRM_T, KC_K, COMBO_END}; // TK -> Q. Use q + Magic for qu.

// Right-hand same-finger vertical combos: alternate homes for x s f j k v.
//
// x is on the MIDDLE finger, not the index, and that is deliberate. The index
// pair is Esc+A, and "Esc then a" is one of the most common sequences in vim --
// leave insert mode, append -- so that chord is the likeliest of the six to be
// struck by accident. Its payload therefore has to be harmless: k just moves
// the cursor up. x deletes a character in normal mode, so it lives on the
// middle-finger pair instead, whose sequence (. then e) is one you essentially
// never type. Swap these two back and a misfire eats your text.
const uint16_t PROGMEM k_combo[] = {KC_ESC,      HRM_A, COMBO_END}; // index top+home
const uint16_t PROGMEM f_combo[] = {HRM_A,       KC_U,  COMBO_END}; // index home+bot
const uint16_t PROGMEM x_combo[] = {P_DOT_SLSH,  HRM_E, COMBO_END}; // mid   top+home
const uint16_t PROGMEM v_combo[] = {HRM_E,       O_NUM, COMBO_END}; // mid   home+bot
const uint16_t PROGMEM j_combo[] = {P_COMM_QUES, HRM_I, COMBO_END}; // ring  top+home
const uint16_t PROGMEM s_combo[] = {HRM_I,       KC_Y,  COMBO_END}; // ring  home+bot

combo_t key_combos[] = {
    [C_Z]  = COMBO(z_combo,  KC_Z),
    [C_QU] = COMBO(qu_combo, KC_Q),
    [C_X]  = COMBO(x_combo,  KC_X),
    [C_F]  = COMBO(f_combo,  KC_F),
    [C_K]  = COMBO(k_combo,  KC_K),
    [C_V]  = COMBO(v_combo,  KC_V),
    [C_J]  = COMBO(j_combo,  KC_J),
    [C_S]  = COMBO(s_combo,  KC_S)
};

uint16_t get_combo_term(uint16_t combo_index, combo_t *combo) {
    switch (combo_index) {
        // Vertical combos. A same-finger pair typed as a sequence (au, eo)
        // is inherently slow, so a short window keeps them out of the way
        // while still being comfortable for a deliberate simultaneous press.
        case C_X:
        case C_F:
        case C_K:
        case C_V:
        case C_J:
        case C_S:
            return 45;
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
    // Older firmware stored _QWERTY as layer 7; after the layer reorder a
    // stale EEPROM default would land on a momentary layer. Reset it.
    if (get_highest_layer(default_layer_state) > _QWERTY) {
        set_single_persistent_default_layer(_BASE);
    }
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

// Caps Word capitalises with WEAK mods, and QMK clears those in action_exec()
// (quantum/action.c:95-98) on every physical press, BEFORE the process_record
// chain. Every custom keycode here then returns false, which skips
// process_caps_word (quantum/quantum.c:355 vs :384 in one && chain), so the
// weak Shift is never re-added. Net effect: mid-CAPS-WORD every completion came
// out lowercase. Anything that emits a letter has to put the Shift back itself.
static inline bool hd_caps_letter(uint16_t keycode) {
    uint16_t basic = QK_MODS_GET_BASIC_KEYCODE(keycode);
    return is_caps_word_on() && basic >= KC_A && basic <= KC_Z;
}

// Erase one character with NO modifiers live. tap_code() emits the CURRENT
// report, so a held Ctrl turned the erase into Ctrl+Backspace and deleted an
// entire WORD; Alt does the same in readline and GTK, and Super goes to the
// compositor under Hyprland. Every replacement-style rule erases before it
// emits, and while the emission helpers all stripped mods, the backspace in
// front of them did not.
static void hd_tap_bspc_unmodded(void) {
    uint8_t mods         = get_mods();
    uint8_t oneshot_mods = get_oneshot_mods();
    uint8_t weak_mods    = get_weak_mods();

    clear_mods();
    clear_oneshot_mods();
    clear_weak_mods();
    send_keyboard_report();

    tap_code(KC_BSPC);

    set_mods(mods);
    set_oneshot_mods(oneshot_mods);
    set_weak_mods(weak_mods);
    send_keyboard_report();
}

static void tap_code16_without_user_shift(uint16_t keycode) {
    uint8_t mods         = get_mods();
    uint8_t oneshot_mods = get_oneshot_mods();
    uint8_t weak_mods    = get_weak_mods();

    // Remove only Shift; preserve Ctrl/Alt/Gui.
    set_mods(mods & ~MOD_MASK_SHIFT);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    // Re-apply Caps Word's shift by hand, but only to letters -- blanket-shifting
    // would turn the punctuation keys' '.' into '>' and ',' into '<'.
    set_weak_mods(hd_caps_letter(keycode) ? (weak_mods | MOD_BIT(KC_LSFT))
                                          : (weak_mods & ~MOD_MASK_SHIFT));
    send_keyboard_report();

    tap_code16(keycode);

    // Restore held physical mods.
    // Do not restore one-shot Shift; this keypress has consumed it.
    set_mods(mods);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    set_weak_mods(weak_mods);
    send_keyboard_report();
}

static inline bool hd_shifted(void) {
    return (get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT;
}

// Literal-string emission. Weak mods are stripped too: a leaked Caps Word Shift
// would rewrite the punctuation inside the string ("'t" becomes '"t'). Callers
// that want an uppercase form pass one -- see hd_magic_word_brief().
static void hd_send_string_unshifted(const char *str) {
    uint8_t mods = get_mods();
    uint8_t oneshot_mods = get_oneshot_mods();
    uint8_t weak_mods = get_weak_mods();
    set_mods(mods & ~MOD_MASK_SHIFT);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    set_weak_mods(weak_mods & ~MOD_MASK_SHIFT);
    send_keyboard_report();
    send_string(str);
    set_mods(mods);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    set_weak_mods(weak_mods);
    send_keyboard_report();
}

// Emit one Compose sequence. Every modifier is stripped for the whole run,
// not just Shift: xkbcommon matches on the character each keypress produces,
// so a held Shift turns ' into " and a held Ctrl suppresses the character
// altogether. Either one breaks the match silently and leaves the Compose key
// armed, swallowing whatever is typed next.
static void hd_compose_send(const hd_compose_seq_t *seq, bool shifted) {
    if (seq == NULL) {
        return;
    }

    uint16_t first  = seq->first;
    uint16_t second = seq->second;
    if (shifted && seq->cased != HD_CASE_NONE) {
        second = S(second);
        if (seq->cased == HD_CASE_BOTH) {
            first = S(first);
        }
    }

    uint8_t mods         = get_mods();
    uint8_t oneshot_mods = get_oneshot_mods();
    uint8_t weak_mods    = get_weak_mods();
    clear_mods();
    clear_oneshot_mods();
    // WEAK mods matter here too, and this is the one place where missing them
    // ate a character. Caps Word's weak Shift turned the ',' of "Compose , c"
    // into '<'; <Compose> < C matches no sequence, so xkbcommon aborted it --
    // and the trigger letter had already been backspaced away. Silent swallow,
    // exactly what the comment above says this function exists to prevent.
    clear_weak_mods();
    send_keyboard_report();

    tap_code(COMPOSE);
    tap_code16(first);
    tap_code16(second);

    // Restore held physical mods. Do not restore one-shot Shift; this
    // character has consumed it.
    set_mods(mods);
    set_oneshot_mods(oneshot_mods & ~MOD_MASK_SHIFT);
    set_weak_mods(weak_mods);
    send_keyboard_report();
}

// step 0 acute, 1 grave, 2 diaeresis -- the three blocks of five in the table.
static const hd_compose_seq_t *hd_accented_vowel_seq(uint16_t base, uint8_t step) {
    uint8_t vowel;
    switch (base) {
        case KC_A: vowel = 0; break;
        case KC_E: vowel = 1; break;
        case KC_I: vowel = 2; break;
        case KC_O: vowel = 3; break;
        case KC_U: vowel = 4; break;
        default:   return NULL;
    }
    return &hd_compose_table[(step % 3) * 5 + vowel];
}

// -----------------------------------------------------------------------------
// Key history shared by Magic and Arcane
// -----------------------------------------------------------------------------
// Both keys read the same "what did you type last" state. hd_magic_pending
// holds a tap-hold key that QMK has not resolved yet, so a fast letter + Magic
// still finds its trigger.

static uint16_t hd_magic_last = KC_NO;
static uint16_t hd_lang_cycle_key = KC_NO;
static uint8_t hd_lang_cycle_step = 0;
static bool hd_lang_cycle_active = false;
static bool hd_lang_cycle_shifted = false;
static uint16_t hd_magic_cycle_key = KC_NO;

#define HD_MAGIC_PENDING_TERM 220

static uint16_t hd_magic_pending = KC_NO;
static uint16_t hd_magic_pending_timer = 0;
static bool     hd_magic_pending_consumed = false;
static uint8_t  hd_magic_last_mods = 0;
static uint8_t  hd_magic_pending_mods = 0;

static void hd_magic_clear_history(void) {
    hd_magic_last = KC_NO;
    hd_magic_pending = KC_NO;
    hd_magic_pending_consumed = false;
    hd_magic_last_mods = 0;
    hd_magic_pending_mods = 0;
    hd_lang_cycle_key = KC_NO;
    hd_lang_cycle_step = 0;
    hd_lang_cycle_active = false;
    hd_lang_cycle_shifted = false;
    hd_magic_cycle_key = KC_NO;
}

// Drop a pending trigger outright. Deliberately NOT folded into
// hd_magic_push(): push also runs from inside the emission helpers, and
// clearing there would stop hd_should_suppress_consumed_pending() from
// recognising the release of the key the completion just consumed -- which
// would then push the trigger letter back over the completion and disarm the
// step-2 cycle. Only a genuinely newer keystroke may invalidate a pending.
static void hd_magic_forget_pending(void) {
    hd_magic_pending = KC_NO;
    hd_magic_pending_consumed = false;
    hd_magic_pending_mods = 0;
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
    // hd_magic_consume_pending() only sets the consumed flag; it deliberately
    // leaves hd_magic_pending alone so the tap-hold release path can still
    // recognise and suppress it. That makes checking the flag here mandatory.
    // Without it a pending trigger stays live for the rest of its 220 ms and a
    // SECOND Magic re-reads it instead of the completion that was just emitted:
    // n + Magic + Magic returned "ngg" rather than "n't", because the step-2
    // test wants trigger == KC_G and kept seeing KC_N. Same for t/l/r ("tss",
    // "lss", "rss"). The timer runs from the LETTER press, not from the first
    // Magic, so whether step 2 worked was decided by how fast the pair was
    // typed -- a race, in the one feature that is supposed to be timer-free.
    if (hd_magic_pending != KC_NO && !hd_magic_pending_consumed &&
        timer_elapsed(hd_magic_pending_timer) <= HD_MAGIC_PENDING_TERM) {
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
    // is_caps_word_on() has to be consulted explicitly: Caps Word's shift is a
    // WEAK mod and QMK has already cleared it by the time this runs. Without
    // it, k + Magic mid-CAPS-WORD replaced the K with a lowercase q, and
    // n + Arcane gave a lowercase enye.
    return is_caps_word_on() ||
           ((trigger_mods | get_mods() | get_oneshot_mods()) & MOD_MASK_SHIFT) != 0;
}

static void hd_magic_push(uint16_t keycode) {
    if (keycode == KC_NO) {
        return;
    }
    hd_magic_last = keycode;
    hd_magic_last_mods = get_mods() | get_oneshot_mods();
    hd_lang_cycle_active = false;
    hd_magic_cycle_key = KC_NO;
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

        // Remaining layer-tap keys.
        case R_NUM: return KC_R;
        case O_NUM: return KC_O;
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

// -----------------------------------------------------------------------------
// Arcane: the dedicated accent key (left middle thumb)
// -----------------------------------------------------------------------------
// Accents used to ride on Magic, which forced a chain of proxy triggers
// (x -> ç, y -> ñ, z -> ß) to dodge collisions with the letter rules, and it
// put the whole accent load behind a right-index key. A separate key removes
// every collision: the natural trigger works directly.
//
//   a/e/i/o/u + Arcane -> á é í ó ú, again -> à è ì ò ù, again -> ä ë ï ö ü
//   n + Arcane -> ñ      c + Arcane -> ç      s + Arcane -> ß
//   1 + Arcane -> ¡      ? + Arcane -> ¿      @ + Arcane -> €    ; + Arcane -> ·
//
// The cycle is timer-free: it advances only because the previous key was also
// Arcane on the same trigger, so it can never stall or fire on its own.

static bool hd_magic_send_compose_replace_or_pending(const hd_compose_seq_t *seq, bool shifted, uint16_t history_key, bool used_pending) {
    if (seq == NULL) {
        return false;
    }

    // Arcane is replacement-style: n+Arcane becomes ñ, not nñ. In this
    // QMK/tap-hold timing even a "pending" tap-hold letter is usually already
    // emitted by the time Arcane runs, so always erase the plain trigger
    // before composing.
    hd_tap_bspc_unmodded();

    hd_compose_send(seq, shifted);
    hd_magic_push(history_key);
    hd_repeat_remember(history_key);

    // Arm the same repeat guard the vowel cycle uses, so a second Arcane on
    // this trigger is a no-op rather than a re-emit. hd_magic_push() clears
    // these, so it has to happen after it. See hd_do_arcane().
    hd_lang_cycle_key    = history_key;
    hd_lang_cycle_active = true;

    if (used_pending) {
        hd_magic_consume_pending();
    }

    return true;
}

// Word briefs: Space + Magic emits a whole word.
//
// This is the one rule whose trigger is a CONTEXT (word-initial) rather than a
// prefix of what it emits, so it lives apart from the letter tables. Only "the"
// earns it, and it is a genuine free lunch: EN dMot -0.35 AND dLeft -0.71pp,
// negative on both axes, which almost nothing else on the board manages. It is
// also the most common word in English, so there is nothing to memorise.
//
// It does NOT make space a decision point -- space alone is untouched; the word
// only appears if Magic follows. Idle in Spanish and Catalan, which costs
// nothing.
//
// hd_magic_cycle_key is armed afterwards so a second Magic is inert. Without
// that, history would sit on the 'e' of "the" and the next Magic would fire
// e -> "es", giving "thees". Arming it is also how a step 2 would be added
// later: test cycle_key == KC_SPC the way the n't rule tests KC_N.
static bool hd_magic_word_brief(uint16_t trigger, bool used_pending) {
    if (trigger != KC_SPC) {
        return false;
    }

    // Pass the cased form rather than shifting the emission: a weak Shift
    // applied across a string would also rewrite any punctuation in it.
    hd_send_string_unshifted(is_caps_word_on() ? "THE" : "the");
    hd_magic_push(KC_E);
    hd_repeat_remember(KC_E);
    hd_magic_cycle_key = KC_SPC;

    if (used_pending) {
        hd_magic_consume_pending();
    }
    return true;
}

// Append-style, like hd_magic_send, but for a two-letter cluster. The LAST
// letter becomes the history key, matching what hd_magic_send does with its
// single keycode.
static bool hd_magic_send2(uint16_t first, uint16_t second) {
    tap_code16_without_user_shift(first);
    tap_code16_without_user_shift(second);
    hd_magic_push(hd_normalize_history_key(second));
    hd_repeat_remember(second);
    return true;
}

static bool hd_magic_replace_with_key(uint16_t keycode, uint16_t history_key, bool used_pending) {
    hd_tap_bspc_unmodded();
    tap_code16_without_user_shift(keycode);
    hd_magic_push(history_key);
    hd_repeat_remember(keycode);

    if (used_pending) {
        hd_magic_consume_pending();
    }

    return true;
}

static inline bool hd_is_arcane_vowel(uint16_t kc) {
    return kc == KC_A || kc == KC_E || kc == KC_I || kc == KC_O || kc == KC_U;
}

static bool hd_arcane_vowel(uint16_t base, bool used_pending) {
    if (!hd_is_arcane_vowel(base)) {
        return false;
    }

    if (hd_lang_cycle_active && hd_lang_cycle_key == base) {
        // Repeated Arcane cycles the accent and replaces the previous one.
        hd_tap_bspc_unmodded();
        hd_lang_cycle_step = (hd_lang_cycle_step + 1) % 3;
        // Do NOT re-derive the case here. hd_magic_push() below has already
        // overwritten hd_magic_last_mods with whatever was held when the
        // previous accent went out, and the Shift that capitalised the
        // trigger was released long before this second press -- so A would
        // give A-acute, then a-grave, then a-diaeresis. The case belongs to
        // the cycle, not to the individual press. A Shift deliberately held
        // during a cycling press can still force upper.
        hd_lang_cycle_shifted |= hd_shifted();
    } else {
        hd_lang_cycle_key = base;
        hd_lang_cycle_step = 0;
        // Latch the case while the trigger's own mods are still on record.
        // This must happen before hd_magic_push() clobbers them below.
        hd_lang_cycle_shifted = hd_magic_trigger_shifted(used_pending);
        // First Arcane replaces the already-emitted plain vowel. This also
        // applies to pending HRM/layer-tap vowels because QMK has usually
        // emitted the tap before Arcane runs.
        hd_tap_bspc_unmodded();
    }

    hd_compose_send(hd_accented_vowel_seq(base, hd_lang_cycle_step),
                    hd_lang_cycle_shifted);

    hd_magic_push(base);
    hd_lang_cycle_key = base;
    hd_lang_cycle_active = true;
    hd_repeat_remember(base);

    if (used_pending) {
        hd_magic_consume_pending();
    }

    return true;
}

static bool hd_do_arcane(void) {
    bool used_pending = false;
    uint16_t trigger = hd_magic_get_trigger(&used_pending);
    if (trigger == KC_NO) {
        return false;
    }

    // A second Arcane on a non-vowel has nothing to cycle to. Without this it
    // would backspace the character it just produced and re-emit it -- and
    // re-derive the case from mods that are no longer held, so a deliberate
    // N-tilde would silently downgrade to n-tilde on the next press. Consume
    // the press and leave the character alone.
    if (hd_lang_cycle_active && hd_lang_cycle_key == trigger && !hd_is_arcane_vowel(trigger)) {
        return true;
    }

    bool shifted = hd_magic_trigger_shifted(used_pending);

    switch (trigger) {
        case KC_A:
        case KC_E:
        case KC_I:
        case KC_O:
        case KC_U:
            return hd_arcane_vowel(trigger, used_pending);

        case KC_N:
            return hd_magic_send_compose_replace_or_pending(HD_SEQ(XC_ENYE), shifted, KC_N, used_pending);    // ñ/Ñ
        case KC_C:
            return hd_magic_send_compose_replace_or_pending(HD_SEQ(XC_CCED), shifted, KC_C, used_pending);    // ç/Ç
        case KC_S:
            return hd_magic_send_compose_replace_or_pending(HD_SEQ(XC_ESZ),  shifted, KC_S, used_pending);    // ß/ẞ

        case KC_1:
            return hd_magic_send_compose_replace_or_pending(HD_SEQ(XC_IEXC), false, KC_1, used_pending);      // ¡
        case S(KC_SLSH):
            return hd_magic_send_compose_replace_or_pending(HD_SEQ(XC_IQUE), false, KC_SLSH, used_pending);   // ¿
        case S(KC_2):
            return hd_magic_send_compose_replace_or_pending(HD_SEQ(XC_EURO), false, S(KC_2), used_pending);   // €
        case KC_SCLN:
        case KC_DOT:
            return hd_magic_send_compose_replace_or_pending(HD_SEQ(XC_MDOT), false, KC_DOT, used_pending);    // ·
    }

    return false;
}

// Letter completions, chosen by measured value on an EN/ES/CA corpus rather
// than by feel. Every rule below either removes motion cost or removes a
// left-hand keystroke; the ones that did neither were cut.
//
//   n + Magic -> g   ng   the single largest left-hand relief available
//   m + Magic -> p   mp   best rule in all three languages
//   t + Magic -> s   ts
//   c + Magic -> k   ck
//   l + Magic -> s   ls   Catalan leans hard on this one
//   r + Magic -> s   rs   pinky relief; costs a little motion
//   x + Magic -> p   xp
//   d + Magic -> v   dv
//   w/g/p + Magic -> h    the three h-digraphs that are close to free
//   q + Magic -> u   qu
//
// t/c/s + Magic -> h were dropped. h lives on the RIGHT pinky home key, so
// those rules never unloaded the left hand at all, and the Magic -> next
// transition they force (Magic and a share the right index; Magic to e is a
// stretch) cost more motion than every other rule here earns combined.
static uint16_t hd_magic_completion(uint16_t trigger) {
    switch (trigger) {
        case KC_N: return KC_G;
        case KC_M: return KC_P;
        case KC_T: return KC_S;
        case KC_C: return KC_K;
        case KC_L: return KC_S;
        case KC_R: return KC_S;
        case KC_X: return KC_P;
        case KC_D: return KC_V;
        case KC_W: return KC_H;
        case KC_G: return KC_H;
        case KC_P: return KC_H;
        case KC_Q: return KC_U;

        // i + Magic -> "is". Best remaining buy on the board: ratio 1.27,
        // which is the same price paid six times over for the vertical combos,
        // and 100% of the relief lands on the LEFT PINKY (s is pinky home),
        // cutting it from 7.19 to 6.78 presses per 100 characters. Stable in
        // all three languages -- EN 1.40 / ES 1.10 / CA 1.15.
        // Chosen over i -> n, which has twice the raw relief (-0.82 vs -0.40)
        // but spends it on the left MIDDLE finger. Pinky relief is the currency
        // this keymap has been buying all along.
        case KC_I: return KC_S;

        // e + Magic -> "es". The largest single relief available anywhere:
        // -1.81 left-hand presses per 100 characters, ALL of it left pinky --
        // roughly a quarter of the entire left-pinky load, 4.4x more than any
        // other rule on the board.
        //
        // Its ratio is 3.87, far past the 1.3 the rest of the table is held to,
        // and that is not hidden here because it is real. But ~90% of that cost
        // is one constant: c(e, MAGIC) = LSB-mild, the right middle-to-index-
        // inner lateral reach, which model3.py's own header calls "the knob".
        // Across the range that file tests, this rule scores anywhere from 1.2
        // (best on the board) to 6.5 (worst). The model cannot settle it; only
        // the hand can. Adopted deliberately, with the pinky load as the reason.
        case KC_E: return KC_S;
    }
    return KC_NO;
}

// Two-letter completions. These emit a cluster rather than a single letter, so
// they need their own table -- hd_magic_completion() is single-keycode by
// contract, and its result is also what gets pushed as the next history key.
//
// Only "sta" earns a slot. Measured end-to-end it is an outright free lunch:
// dMot -0.31 AND dLeft -0.31 (ratio -0.98), stable across EN -0.89 / ES -1.00 /
// CA -0.99, and P(t|s) ranks 2nd-3rd of every continuation of s. Emitting the
// 'a' inside the cluster is also what dodges the Magic-then-a same-finger
// collision that ruins the single-letter s -> t rule (both are right index).
//
// Honest caveat: the keystroke it removes is left INDEX home, the strongest and
// cheapest key on that hand. This is volume relief, not strain relief.
static bool hd_magic_completion2(uint16_t trigger, uint16_t *first, uint16_t *second) {
    switch (trigger) {
        case KC_S: *first = KC_T; *second = KC_A; return true;   // s + Magic -> "sta"
    }
    return false;
}

// Second-step completions. Pressing Magic twice on the same trigger replaces
// the first completion with a second one. Like the Arcane accent cycle this is
// timer-free: the only way to reach step 2 is another Magic press, so it can
// never stall waiting for a tap and can never fire on its own.
//
// Only n uses it so far: n + Magic -> "ng", n + Magic + Magic -> "n't". It is
// worth a step here because the alternative spelling of n't needs the right
// pinky TOP row plus a left-index t, while step 2 is one more press of a key
// the finger is already on. It is NOT worth doing for ordinary letters, where
// pressing the letter directly always beats a Magic press plus a backspace.

static bool hd_do_base_magic(void) {
    bool used_pending = false;
    uint16_t trigger = hd_magic_get_trigger(&used_pending);
    if (trigger == KC_NO) {
        return false;
    }

    if (hd_magic_cycle_key == KC_N && trigger == KC_G) {
        hd_tap_bspc_unmodded();                        // erase the g of "ng"
        hd_send_string_unshifted("'t");           // n + Magic + Magic -> n't
        hd_magic_push(KC_T);
        hd_repeat_remember(KC_T);
        return true;
    }

    // Past this point a completion has NOT been armed for step 2, so a second
    // Magic would re-enter with the completion itself as the trigger --
    // hd_magic_send() pushes what it emitted as the new history key. Several
    // completions are themselves triggers, which produced silent nonsense:
    // m + Magic + Magic -> "mph" (m->p, then p->h), x + Magic + Magic -> "xph",
    // and worst, c + Magic + Magic -> "cq", because the emitted k hit the
    // replacement-style k->q rule and ATE a character that was already correct.
    // n't is checked above precisely because it is the one real step-2 rule.
    // hd_magic_cycle_key is cleared by hd_magic_push() on any ordinary
    // keystroke, so this only ever fires on a genuine back-to-back Magic.
    if (hd_magic_cycle_key != KC_NO) {
        return false;
    }

    if (hd_magic_word_brief(trigger, used_pending)) {
        return true;
    }

    // k + Magic -> q keeps q off the symbol layer. It is replacement-style,
    // so it has to run before the append-style completions.
    if (trigger == KC_K) {
        return hd_magic_replace_with_key(hd_magic_trigger_shifted(used_pending) ? S(KC_Q) : KC_Q, KC_Q, used_pending);
    }

    // Two-letter completions run before the single-letter table; nothing is
    // armed for step 2 afterwards, so a following Magic is inert.
    uint16_t c2_first, c2_second;
    if (hd_magic_completion2(trigger, &c2_first, &c2_second)) {
        hd_magic_send2(c2_first, c2_second);
        if (used_pending) {
            hd_magic_consume_pending();
        }
        return true;
    }

    // Letter completions are append-style, so the pending and settled paths are
    // identical: send only the completion. Sending the whole digraph would turn
    // a fast t + Magic into "tts".
    uint16_t completion = hd_magic_completion(trigger);
    if (completion != KC_NO) {
        hd_magic_send(completion);            // this clears hd_magic_cycle_key
        hd_magic_cycle_key = trigger;         // ...so arm step 2 afterwards
        if (used_pending) {
            hd_magic_consume_pending();
        }
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
    }

    return false;
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
        case REP_NAV:
        case HD_MAGIC:
        case HD_ARCANE:
        case QK_BOOT:
            return false;
    }
    return true;
}

// -----------------------------------------------------------------------------
// Main key processing
// -----------------------------------------------------------------------------

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Every custom keycode below returns false, which skips
    // process_record_handler -- and that is also where QMK retires a one-shot
    // layer once another key is pressed. Retire it here instead, or a tapped
    // Lang stays armed through the character it was meant to produce and the
    // next keystroke comes off the wrong layer. The call is idempotent and
    // does nothing when no one-shot layer is active, so the range check only
    // needs to exclude OSL itself.
    if (record->event.pressed && keycode >= SAFE_RANGE && keycode < XC_LAST) {
        clear_oneshot_layer_state(ONESHOT_OTHER_KEY_PRESSED);
    }

    switch (keycode) {
        case REP_NAV:
            // REP_NAV = LT(_NAV, KC_F24). The tap side must never reach QMK.
            // Returning true on the press let process_action register KC_F24 --
            // a real basic keycode, so it entered the HID report -- and the
            // release then returned false, skipping the matching
            // unregister_code. F24 stayed latched FOREVER, after every single
            // Repeat tap, permanently burning one of six boot-protocol rollover
            // slots (there is no NKRO here) and putting a phantom key in every
            // later report.
            //
            // The old `case KC_F24:` arm below this was dead code:
            // process_record_quantum resolves the keycode with
            // get_record_keycode(), which only yields the tap keycode for
            // combo- and repeat-synthesised records, never a matrix event. So
            // process_record_user always saw REP_NAV, and the
            // hd_rep_nav_repeat_sent double-fire guard guarded nothing.
            //
            // Hold side still returns true, because QMK drives layer_on/off.
            if (record->tap.count) {
                if (record->event.pressed) {
                    hd_do_repeat();
                }
                return false;
            }
            return true;

        case HD_MAGIC:
            if (record->event.pressed) {
                hd_do_base_magic();
            }
            return false;

        case HD_ARCANE:
            if (record->event.pressed) {
                hd_do_arcane();
            }
            return false;

        // Language layer. These emit a character outright rather than
        // replacing a trigger, so unlike Arcane there is no backspace.
        case XC_AACU ... XC_MDOT:
            if (record->event.pressed) {
                hd_compose_send(HD_SEQ(keycode), hd_shifted() || is_caps_word_on());
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

        case KC_ESC:
        case KC_ENT:
            if (record->event.pressed) {
                hd_magic_clear_history();
            }
            break;

        case P_DOT_SLSH:
            if (record->event.pressed) {
                // Punctuation ends Caps Word. QMK's default
                // caps_word_press_user() would do that, but this key returns
                // false and so never reaches process_caps_word -- Caps Word
                // stayed stuck on past the end of the word, with no idle
                // timeout configured to rescue it.
                caps_word_off();
                uint16_t out = hd_shifted() ? KC_SLSH : KC_DOT;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? KC_SLSH : KC_DOT);
                hd_repeat_remember(out);
            }
            return false;

        case P_COMM_QUES:
            if (record->event.pressed) {
                // Punctuation ends Caps Word. QMK's default
                // caps_word_press_user() would do that, but this key returns
                // false and so never reaches process_caps_word -- Caps Word
                // stayed stuck on past the end of the word, with no idle
                // timeout configured to rescue it.
                caps_word_off();
                uint16_t out = hd_shifted() ? S(KC_SLSH) : KC_COMM;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? S(KC_SLSH) : KC_COMM);
                hd_repeat_remember(out);
            }
            return false;

        case P_QUOT_DQUO:
            if (record->event.pressed) {
                // Punctuation ends Caps Word. QMK's default
                // caps_word_press_user() would do that, but this key returns
                // false and so never reaches process_caps_word -- Caps Word
                // stayed stuck on past the end of the word, with no idle
                // timeout configured to rescue it.
                caps_word_off();
                uint16_t out = hd_shifted() ? S(KC_QUOT) : KC_QUOT;
                tap_code16_without_user_shift(out);
                hd_magic_push(hd_shifted() ? S(KC_QUOT) : KC_QUOT);
                hd_repeat_remember(out);
            }
            return false;
    }

    // Track normal taps for Magic / Arcane and private Repeat
    if (record->event.pressed) {
        uint16_t hist = hd_normalize_history_key(keycode);

        uint16_t rep  = hd_repeat_normalize_key(keycode);

        switch (keycode) {
            case HRM_S: case HRM_C: case HRM_N: case HRM_T:
            case HRM_A: case HRM_E: case HRM_I: case HRM_H:
            case R_NUM: case O_NUM: case SPC_SYM:
                // Only a TAP may arm a trigger. QMK delivers the press record
                // once the tap-hold has resolved, with tap.count == 0 for a
                // hold (quantum/action_tapping.c) -- and without this test a
                // key merely HELD as a modifier or layer armed its tap letter.
                // Holding SPC_SYM to type "->" then pressing Magic injected the
                // word "the"; holding HRM_A as Shift then pressing Arcane
                // backspaced the letter just typed and replaced it with a
                // lowercase accent. A hold must also actively clear any pending
                // it displaced, or the stale one survives its full 220 ms.
                if (hist != KC_NO && record->tap.count > 0) {
                    hd_magic_set_pending(hist);
                    hd_repeat_set_pending(hist);
                } else {
                    hd_magic_forget_pending();
                }
                break; // tap-hold keys are handled on release if tapped.
            default:
                if (hist != KC_NO) {
                    // A settled plain key supersedes any pending tap-hold
                    // trigger. hd_magic_get_trigger() prefers pending
                    // unconditionally, so without this the letter you actually
                    // typed last stayed invisible to Magic for 220 ms: "e x
                    // Magic" gave "exs" instead of "exp", because the stale e
                    // won. Reproduces around 75-80 wpm.
                    hd_magic_forget_pending();
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
            // Skip the bookkeeping when a completion already consumed this key,
            // but do NOT swallow the event. Returning false here skipped
            // process_record_handler, which is where QMK retires the tap
            // (quantum/action.c) -- so the tapped letter was registered and
            // never unregistered, staying latched in the HID report. The next
            // press of that letter then produced no transition and was silently
            // eaten, and one of six boot-protocol rollover slots was gone.
            // The character itself was already emitted on the press, so there
            // was never anything here worth suppressing except the duplicate
            // push and remember.
            if (hist != KC_NO && !hd_should_suppress_consumed_pending(keycode)) {
                hd_magic_push(hist);
                hd_repeat_remember(hist);
            }
        }
    }

    return true;
}
// Only _BASE and _SYM carry ASCII art below; open layers.html in this folder
// for a drawn diagram of all eight layers, in physical column order.
//
// Reading the arrays: rows 0 and 1 run  L[pinky ring mid index inner extra]
// then R[extra inner index mid ring pinky];  row 2 drops the extra keys;
// thumbs run L[outer mid inner] then R[inner mid outer]. The right half is
// stored innermost-first, so its last entry is the right pinky.
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /*
     * Layer 0 — Vibranium-f + Vim/terminal-oriented base
     *
     *   X W M G J   ||   Enter   Esc    ./     ,?    '"
     *   S C N T K   ||   Magic    A      E      I     H
     *   F P L D V   ||     R      U    O/Num    Y     B
     *
     *   extra cols   Lang / Sys        ||   Tab / Sys
     *   thumbs       CapsWord Arcane R/Num  ||  Repeat/Nav Space/Sym Bspc
     *
     * Chords:
     * - JG -> Z,  TK -> Q  (q + Magic -> qu)
     * - right-hand same-finger vertical combos -> alternate homes for the
     *   costly left-hand letters:
     *       Esc+A -> X     A+U -> F
     *       ./+E  -> K     E+O -> V
     *       ,?+I  -> J     I+Y -> S
     */
    [_BASE] = LAYOUT_split_3x5_3_ex2(

        KC_X,    KC_W,    KC_M,    KC_G,    KC_J,    LANG,       KC_TAB,  KC_ENT,    KC_ESC,   P_DOT_SLSH,  P_COMM_QUES, P_QUOT_DQUO,
        HRM_S,   HRM_C,   HRM_N,   HRM_T,   KC_K,    SYS,        SYS,     HD_MAGIC,  HRM_A,    HRM_E,       HRM_I,       HRM_H,
        KC_F,    KC_P,    KC_L,    KC_D,    KC_V,                         KC_R,      KC_U,     O_NUM,       KC_Y,        KC_B,
                                   CW_TOGG, HD_ARCANE, R_NUM,    REP_NAV, SPC_SYM, KC_BSPC
    ),

    /*
     * Layer 1 — Nav/Edit
     */
    [_NAV] = LAYOUT_split_3x5_3_ex2(
        UNDO,    CUT,     COPY,    PASTE,   SAVE,    _______,     _______, KC_PGUP, KC_HOME, KC_UP,   KC_END,  KC_DEL,
        KC_LALT, KC_LCTL, KC_LGUI,  KC_LSFT, KC_TAB,  _______,     _______, KC_PGDN, KC_LEFT, KC_DOWN, KC_RGHT, KC_BSPC,
        FIND,    S_TAB,   PREVT,   NEXTT,   ALT_TAB,                         KC_INS,  WORD_L,  WORD_R,  C_BSPC,  C_DEL,
                                    _______, _______, _______,   QK_AREP, _______, _______
    ),

    /*
     * Layer 2 — Symbols          (hold Space, right middle thumb)
     *
     *   ! @ # $ %   ||   XXXX   q    |    \    z
     *   ^ & * + =   ||     -    (    {    [    <
     *   ~ - _ : ;   ||     `    )    }    ]    >
     *
     *   extra cols and all six thumbs: transparent
     *
     * Brackets are stacked in pairs: open on the home row, close directly
     * below it.  q and z are the slow path -- T+K and J+G are the fast one.
     */
    [_SYM] = LAYOUT_split_3x5_3_ex2(
        S(KC_1),   S(KC_2),   S(KC_3),   S(KC_4),   S(KC_5),   _______,     _______, XXXXXXX,    KC_Q,    S(KC_BSLS), KC_BSLS,    KC_Z,
        S(KC_6),   S(KC_7),   S(KC_8),   S(KC_EQL), KC_EQL,    _______,     _______, KC_MINS,    S(KC_9),    S(KC_LBRC), KC_LBRC,    S(KC_COMM),
        S(KC_GRV), KC_MINS,   S(KC_MINS), S(KC_SCLN), KC_SCLN,                       KC_GRV,     S(KC_0),    S(KC_RBRC), KC_RBRC,    S(KC_DOT),
                                          _______, _______, _______,                      _______, _______, _______
    ),

    /*
     * Layer 3 — Numbers/Fn
     */
    [_NUM] = LAYOUT_split_3x5_3_ex2(
        KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   _______,     _______, KC_SLSH, KC_7,    KC_8,    KC_9,    KC_MINS,
        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  _______,     _______, S(KC_8), KC_4,    KC_5,    KC_6,    S(KC_EQL),
        KC_F11,  KC_F12,  KC_F13,  KC_F14,  KC_F15,                          KC_DOT, KC_1,    KC_2,    KC_3,    KC_0,
                                      _______, _______, _______,     _______, _______, _______
    ),

    /*
     * Layer 4 — Language / Compose
     *
     *   —  —  ñ  ç  ß   ||   á  é  í  ó  ú
     *   —  —  ¿  ·  €   ||   à  è  ì  ò  ù
     *   —  —  ¡  —  —   ||   ä  ë  ï  ö  ü
     *
     * Shift gives the capital where one exists. Everything here is a
     * duplicate of what Arcane already reaches from the home row; the layer
     * is the explicit fallback for when you want a character without having
     * just typed its base letter.
     */
    [_LANG] = LAYOUT_split_3x5_3_ex2(
        _______, _______, XC_ENYE, XC_CCED, XC_ESZ,  _______,     _______, XC_AACU, XC_EACU, XC_IACU, XC_OACU, XC_UACU,
        _______, _______, XC_IQUE, XC_MDOT, XC_EURO, _______,     _______, XC_AGRV, XC_EGRV, XC_IGRV, XC_OGRV, XC_UGRV,
        _______, _______, XC_IEXC, _______, _______,                       XC_AUML, XC_EUML, XC_IUML, XC_OUML, XC_UUML,
                                      _______, _______, _______,     _______, _______, _______
    ),

    /*
     * Layer 5 — System
     */
    [_SYS] = LAYOUT_split_3x5_3_ex2(
        XXXXXXX, XXXXXXX, VIBRA,   QWERTY,  MOUSE_TG, QK_BOOT,    QK_BOOT, KC_PSCR, KC_SCRL, KC_PAUS, XXXXXXX, XXXXXXX,
        KC_MUTE, KC_VOLD, KC_VOLU, KC_BRID, KC_BRIU, _______,     _______, KC_MPRV, KC_MPLY, KC_MNXT, KC_MSTP, XXXXXXX,
        CM_TOGG, XXXXXXX, XXXXXXX, KC_NUM,  COMPOSE,                        SH_TOGG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
                                      _______, _______, _______,     _______, _______, _______
    ),

    /*
     * Layer 6 — Mouse / scroll
     */
    [_MOUSE] = LAYOUT_split_3x5_3_ex2(
        _______, _______, _______, _______, MOUSE_TG, _______,      _______, MS_WHLL, MS_WHLU, MS_WHLR, _______, _______,
        _______, _______, _______, _______, _______, _______,      _______, MS_LEFT, MS_DOWN, MS_UP,   MS_RGHT, MS_BTN2,
        _______, _______, _______, _______, _______,               _______, MS_BTN1, MS_WHLD, MS_BTN3, _______,
                                      _______, _______, _______,  _______, _______, _______
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
