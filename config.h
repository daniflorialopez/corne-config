#pragma once

#define TAPPING_TERM 250
#define QUICK_TAP_TERM 0
#define FLOW_TAP_TERM 150

#define COMBO_TERM 55
#define COMBO_TERM_PER_COMBO
#define COMBO_MUST_TAP_PER_COMBO

#define PERMISSIVE_HOLD
#define CHORDAL_HOLD

// No UNICODE_SELECTED_MODES: accented characters go out as Compose sequences
// instead of Unicode codepoints. See the Compose block at the top of keymap.c
// for why, and for the one xkb option this depends on.
