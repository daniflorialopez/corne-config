# Corne Vibranium Magic Keymap

This is a QMK keymap for a Corne `crkbd/rev4_1/mini`, built around a Hands Down Vibranium-f-style base layout plus a small set of ergonomic firmware features:

- Magic key for digraphs, language characters, and small symbol completions.
- Repeat/Nav thumb key: tap repeats, hold opens Nav.
- Subhome layer holds for mirrored Nav/Sym/Num access.
- Spanish/Catalan/German-friendly Unicode through Magic, with the old language layer kept as a fallback.
- Small Vim/terminal-oriented navigation, symbol, number, system, and mouse layers.

The guiding principle is not maximum cleverness. The goal is a calm, low-strain layout that keeps the most-used actions close while avoiding large text expansions or editor-specific snippets.

## Compile target

```bash
qmk compile -kb crkbd/rev4_1/mini -km vibranium_f
```

```

## Layer overview

```c
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
```

Default layers (`_BASE`, `_QWERTY`) must stay below all momentary layers:
the default layer is looked up at its bit position, so a high default layer
would shadow every layer beneath it.

| Layer | Purpose |
|---|---|
| `_BASE` | Main Vibranium-f-style typing layer |
| `_QWERTY` | Rescue QWERTY layer |
| `_NAV` | Navigation, editing, tab/window movement, layer lock |
| `_SYM` | Symbols and programming punctuation |
| `_NUM` | Numbers, function keys, numeric punctuation |
| `_LANG` | Legacy/fallback Unicode layer |
| `_SYS` | System/media/boot/default-layer/mouse-toggle controls |
| `_MOUSE` | Firmware mouse and scroll fallback |

## Base layout

Current base layer, conceptually:

```text
X W M G J | Lang      || Tab Enter Esc ./ ,? '"
S C N T K | Sys       || Sys Magic A E I H
F P L/Num D/Nav V/Sym || SkipMagic U/Sym O/Nav Y/Num B
CapsWord Boring R/Num || Repeat/Nav Space/Sym Backspace
```

Important base keys:

| Key | Behavior |
|---|---|
| `HD_MAGIC` | Main Magic key |
| `HD_SKIP_MAGIC` | Experimental Skip Magic key |
| `REP_NAV` | Tap = private Repeat, hold = Nav |
| `R_NUM` | Tap `r`, hold Num |
| `SPC_SYM` | Tap Space, hold Sym |
| `D_NAV`, `O_NAV` | Subhome Nav holds |
| `V_SYM`, `U_SYM` | Subhome Sym holds |
| `L_NUM`, `Y_NUM` | Subhome Num holds |
| `BORING` | Toggle Boring Mode |
| `CALM` | Toggle Calm Mode from Sys layer |

## Home-row mods

Left hand:

| Key | Hold |
|---|---|
| `S` | Alt |
| `C` | Ctrl |
| `N` | Gui/Super |
| `T` | Shift |

Right hand:

| Key | Hold |
|---|---|
| `A` | Shift |
| `E` | Gui/Super |
| `I` | Ctrl |
| `H` | Alt |

## Thumb strategy

The right thumb keeps the most important hold:

```text
Repeat/Nav  Space/Sym  Backspace
```

The left thumb gives:

```text
CapsWord  Boring  R/Num
```

The design intentionally keeps Nav on the comfortable right inner thumb. Do not weaken Nav responsiveness just to fix rare Repeat edge cases.

## Combos

Tap-only smart combos:

| Combo | Output |
|---|---|
| `T + N` | `th` |
| `C + N` | `ch` |
| `S + C` | `sh` |
| `W + M` | `wh` |
| `G + M` | `gh` |
| `F + P` | `ph` |
| `J + G` | `z` |
| `T + K` | `q` |

The old `TK -> qu` behavior was intentionally changed. Use:

```text
T + K -> q
q + Magic -> qu
```

This avoids the old automatic `u` behavior.

## Magic key

Magic is the main adaptive key. It uses recent key history and pending tap-hold context.

### H-digraph Magic

| Input | Output |
|---|---|
| `t + Magic` | `th` |
| `c + Magic` | `ch` |
| `s + Magic` | `sh` |
| `w + Magic` | `wh` |
| `g + Magic` | `gh` |
| `p + Magic` | `ph` |

The old combos remain available. Use whichever path feels calmer.

### Q Magic

| Input | Output |
|---|---|
| `q + Magic` | `qu` |
| `k + Magic` | `q` |
| `Shift+k + Magic` | `Q` |

### Direct language Magic

Language Magic is now available through the normal Magic key. The old `_LANG` layer remains as a fallback.

Vowels cycle acute → grave → umlaut:

| Input | Cycle |
|---|---|
| `a + Magic` | `á`, then `à`, then `ä` |
| `e + Magic` | `é`, then `è`, then `ë` |
| `i + Magic` | `í`, then `ì`, then `ï` |
| `o + Magic` | `ó`, then `ò`, then `ö` |
| `u + Magic` | `ú`, then `ù`, then `ü` |

Proxy language keys avoid conflicts with useful English/Magic rules:

| Input | Output |
|---|---|
| `x + Magic` | `ç` |
| `y + Magic` | `ñ` |
| `z + Magic` | `ß` |

Punctuation/symbol Magic:

| Input | Output |
|---|---|
| `1 + Magic` | `¡` |
| `? + Magic` | `¿` |
| `@ + Magic` | `€` |
| `; + Magic` | `·` |

### Small English snippets

Keep these small. Do not add framework/editor snippets here.

| Input | Output |
|---|---|
| `n + Magic` | `n't` |
| `h + Magic` | `have` |
| `Space + Magic` | `the` |

`i + Magic` currently prioritizes accented vowels, not English `I`.

### Code Magic

Small symbol completions only:

| Input | Output |
|---|---|
| `. + Magic` | `./` |
| `- + Magic` | `->` |
| `/ + Magic` | `/*` |
| `* + Magic` | `*/` |
| `, + Magic` | comma + space |
| `: + Magic` | colon + space |
| `! + Magic` | `!=` |
| `< + Magic` | `<=` |
| `> + Magic` | `>=` |
| `= + Magic` | `==` |
| `~ + Magic` | `~/` |
| `$ + Magic` | `${}` |

## Skip Magic

Skip Magic is experimental. It exists on the base layer but should be observed before being expanded.

Current idea:

```text
key1 key2 SkipMagic -> key1 key2 key1
```

Specific rules currently include previous-key mappings such as `o -> e`, `i -> y`, `n -> l`, `t -> d`, and `c -> p`.

If Skip Magic does not earn its key in real use, it is a candidate for demotion or replacement.

## Repeat/Nav

`REP_NAV` is defined as:

```c
#define REP_NAV LT(_NAV, KC_F24)
```

Behavior:

| Action | Result |
|---|---|
| Tap `REP_NAV` | Private Repeat |
| Hold `REP_NAV` | Nav layer |

Known priority:

```text
Nav quality > perfect ultra-fast Repeat edge cases
```

Do not make Nav worse to fix rare Repeat timing issues.

## Nav layer

Nav layer includes:

- Arrows.
- Home/End/PageUp/PageDown.
- Backspace/Delete.
- Word left/right.
- Ctrl+Backspace/Ctrl+Delete.
- Undo/Cut/Copy/Paste/Save/Find.
- Previous/next tab and Alt-Tab style actions.
- Alt Repeat (`QK_AREP`).
- Layer Lock (`QK_LLCK`).

## Symbol layer

Symbol layer includes programming punctuation and redundant rescue access to rare letters.

Recent intentional additions:

- `q` on the right-hand top row.
- `z` on the right-hand top row.
- hyphen on a right-hand blank.

## Number layer

Number layer includes function keys, numbers, and numeric punctuation.

Recent intentional change:

- The former right-hand `%` position became `.` because dot is more useful for numeric entry.

## System layer

System layer includes:

- Unicode mode selection: Linux / Windows.
- Default layer switching: Vibranium / QWERTY.
- Bootloader.
- Media keys.
- Brightness.
- PrintScreen / ScrollLock / Pause.
- Calm Mode.
- OS mode placeholder.
- Mouse layer toggle.
- Swap Hands toggle if enabled/supported.

## Mouse layer

The mouse layer is a fallback, not the main pointer strategy.

It includes:

- Pointer movement.
- Scroll wheel directions.
- Left/middle/right click.
- Mouse toggle exit key.

Use Vimium/browser tools and OS/window-manager pointer helpers for serious mouseless control. Keep QMK mouse as fallback.

## Modes

### Boring Mode

Boring Mode disables Magic behavior and gives a safer/plain mode.

Use when:

- Magic is interfering with an application.
- You want a more literal keyboard temporarily.
- You are debugging.

### Calm Mode

Calm Mode is intended to reduce clever behavior while preserving the most ergonomic completions. It may need refinement after real-world use.

## Unicode notes

Unicode works by sending the OS/application Unicode input sequence. Browser/GUI apps may work correctly while some terminals show the raw hex-like sequence. If Unicode works in the browser but not in a terminal, treat it as an application/input-method compatibility issue before blaming the firmware.
