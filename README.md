# Vibranium-f — Corne rev4.1 mini

A QMK keymap for `crkbd/rev4_1/mini` (40 keys), built on a Hands Down
Vibranium-f base with an adaptive Magic key, a dedicated accent key, and a
family of same-finger vertical chords.

```bash
qmk compile -kb crkbd/rev4_1/mini -km vibranium_f
```

**Open `layers.html` for a drawn diagram of all eight layers.** It is a single
self-contained file — no CDN, no fonts, no build step — so it works offline and
can be sent to anyone. The ASCII art in this file and in `keymap.c` covers only
the two layers worth memorising.

## The constraint

The left hand has chronic tendonitis. That is the axis every decision is scored
against, and it is why several choices look strange out of context:

- Load on the left hand is worth paying motion cost to remove. Load on the
  right hand is not, because the right hand is fine.
- Anything that can misfire is worse than anything that is merely slow. A
  mistimed hold that emits the wrong character costs more than a wasted
  keystroke, so timing-sensitive features were removed rather than tuned.
- Two ways to type the same letter is a feature, not redundancy, as long as one
  of the two is healthier.

Rules were chosen by measuring an English (Norvig 1-grams), Spanish and Catalan
(OpenSubtitles) corpus through a model of this specific layout, not by feel.
Against the previous revision the current set cuts bad motions (same-finger
bigrams, scissors, lateral stretches) by about 42% in English and 61% in
Spanish and Catalan, and takes roughly 25% off the left pinky.

## Layers

```c
enum layers { _BASE, _QWERTY, _NAV, _SYM, _NUM, _LANG, _SYS, _MOUSE };
```

The two default layers must stay first. A default layer is looked up at its bit
position, so a high default layer shadows every layer beneath it — putting
`_QWERTY` last is what broke layer switching once already.

| Layer | Reached by | Purpose |
|---|---|---|
| `_BASE` | default | Vibranium-f typing layer |
| `_QWERTY` | Sys → QWERTY (persistent) | Rescue layer for guests and scancode-reading software |
| `_NAV` | hold Repeat, right inner thumb | Arrows, editing, tab and window motion |
| `_SYM` | hold Space, right middle thumb | Symbols and programming punctuation |
| `_NUM` | hold R (left inner thumb) **or** hold O | Numbers and F1–F15 |
| `_LANG` | tap Lang, left extra column top | One-shot Compose layer; explicit fallback to Arcane |
| `_SYS` | hold Sys, either extra column home | Media, brightness, boot, layer switching |
| `_MOUSE` | toggled from Sys | Pointer fallback |

## Base layer

```text
  X W M G J   ||   Enter   Esc    ./     ,?    '"
  S C N T K   ||   Magic    A      E      I     H
  F P L D V   ||     R      U    O/Num    Y     B

  extra cols   Lang / Sys        ||   Tab / Sys
  thumbs       CapsWord Arcane R/Num  ||  Repeat/Nav Space/Sym Bspc
```

Note `h` is on the **right** pinky home key, not the left. This matters more
than it sounds: it is why h-digraph completions were never relieving the injured
hand, and why most of them were dropped.

`r` appears twice — on the left inner thumb as `LT(_NUM, KC_R)` and on the right
index-inner bottom row. Whichever hand is free.

### Home-row mods

| Left | Hold | | Right | Hold |
|---|---|---|---|---|
| `S` | Alt | | `A` | Shift |
| `C` | Ctrl | | `E` | Gui |
| `N` | Gui | | `I` | Ctrl |
| `T` | Shift | | `H` | Alt |

Chordal Hold plus Flow Tap keep these out of the way during fast same-hand runs.
These are the **only** mod-taps left: every subhome layer-tap on a letter was
removed, because a mistimed hold on a letter is the misfire that costs most. The
sole survivor is `O` for `_NUM`, kept so the right hand can reach numbers while
the left hand is on the F-keys.

## Chords

All chords are tap-only (`get_combo_must_tap` returns true), so holding the keys
still gives you the underlying mods and layers.

**Six same-finger vertical chords** on the right hand. One finger physically
cannot roll vertically at typing speed, so no ordinary letter sequence can
produce them — this is the structural reason they are safe where the old
horizontal digraph combos were not. Each is an *alternate* home for a left-hand
letter that is expensive to reach; the base key still works normally.

| Press together | Gives | Finger |
|---|---|---|
| `Esc` + `A` | `k` | right index, top + home |
| `A` + `U` | `f` | right index, home + bottom |
| `./` + `E` | `x` | right middle, top + home |
| `E` + `O` | `v` | right middle, home + bottom |
| `,?` + `I` | `j` | right ring, top + home |
| `I` + `Y` | `s` | right ring, home + bottom |

`x` sits on the middle finger rather than the index on purpose. `Esc` then `a`
is one of the most common sequences in vim — leave insert mode, append — which
makes the index chord the likeliest of the six to be struck by accident, so its
payload has to be harmless. `k` only moves the cursor up; `x` would delete a
character. `x` therefore lives on the middle-finger pair, whose sequence (`.`
then `e`) is one you essentially never type.

Rule of thumb for when to use one: **chord when the letter before it was on the
left hand.** After a right-hand vowel the plain key always wins — `use` and `usa`
are already perfect alternation, and chording makes them same-hand.

**Two horizontal chords** on the left hand, between the index and inner columns:

| Press together | Gives |
|---|---|
| `J` + `G` | `z` |
| `T` + `K` | `q` — then `q + Magic` for `qu` |

Windows: 45 ms for the vertical six, 55 ms for the left-hand two.

## Magic

`HD_MAGIC` sits on the right index-inner home key. It appends a completion to
whatever you typed last, reading both settled taps and still-unresolved tap-hold
keys, so a fast `t + Magic` still finds its trigger.

| Type | Gives | EN freq | Note |
|---|---|---|---|
| `Space + Magic` | `the` | 0.705% | word brief — **free lunch**, saves motion *and* left load |
| `e + Magic` | `es` | 0.990% | biggest relief on the board — ~25% of left-pinky load |
| `n + Magic` | `ng` | 0.631% | largest single-letter relief |
| `i + Magic` | `is` | 0.404% | all relief on the left pinky |
| `s + Magic` | `sta` | 0.313% | **free lunch** — saves motion *and* left-hand load |
| `r + Magic` | `rs` | 0.298% | left-pinky relief; costs a little motion |
| `t + Magic` | `ts` | 0.281% | |
| `c + Magic` | `ck` | 0.158% | |
| `m + Magic` | `mp` | 0.154% | best-scoring rule in all three languages |
| `w + Magic` | `wh` | 0.137% | |
| `g + Magic` | `gh` | 0.129% | |
| `l + Magic` | `ls` | 0.120% | carried by Catalan |
| `p + Magic` | `ph` | 0.080% | |
| `x + Magic` | `xp` | 0.037% | |
| `d + Magic` | `dv` | 0.035% | advance, advice, advantage |
| `q + Magic` | `qu` | — | |
| `k + Magic` | `q` (replaces the `k`) | — | keeps `q` off the symbol layer; Shift gives `Q` |
| `n + Magic + Magic` | `n't` | — | second press rewrites `ng` into `n't` |

`t/c/s + Magic → h` were **dropped**. Since `h` is on the right pinky, those
rules removed no left-hand work at all, while the follow-through they force
(Magic and `a` share the right index; Magic to `e` is a lateral stretch) cost
more motion than every other rule here earns combined — `t → h` alone was worse
than the rest of the table put together. `th`, `ch` and `sh` need no
replacement: left-hand letter into right pinky is already one of the cheapest
transitions on the board. Just type the `h`.

`w/g/p → h` survive because they are close to free, and because the muscle
memory was already there.

`e + Magic → es` is the one rule adopted **against** its ratio. At 3.87 it is
nearly three times the 1.3 the rest of the table is held to. It is here because
it removes 1.81 left-hand presses per 100 characters, all of them left pinky —
about a quarter of the entire load on the worst finger, and 4.4× more than
anything else available. Roughly 90% of its cost is a single constant,
`LSBM_COST` in `model3.py`, which that file's own header calls "the knob": the
right middle-to-index-inner lateral reach. Across the range the file itself
tests, this rule scores from 1.2 (best on the board) to 6.5 (worst). No corpus
settles it. If the reach to Magic from `e` starts to feel like work, this is the
first rule to pull.

Note that `e` also carries Arcane (`é è ë`). They are different keys, so there
is no functional collision — but `e` is now the only letter with two adaptive
behaviours on it, and it is the most frequent letter in all three languages.

### Second-step completions

Pressing Magic twice on the same trigger replaces the first completion with a
second. Timer-free — the only way to reach step 2 is another Magic press, so it
cannot stall waiting for a tap and cannot fire on its own.

Only `n` uses it. It pays there because the alternative spelling of `n't` needs
the right-pinky top row plus a left-index `t`, whereas step 2 is one more press
of a key the finger is already on. It does **not** pay for ordinary letters,
where pressing the letter directly always beats a Magic press plus a backspace.

A second Magic press costs an extra `c(MAGIC, MAGIC)` = 2.0 — pressing the same
key twice. An earlier model omitted that term and made step-2 letter rules look
affordable; corrected, `i + Magic + Magic → is` scores 3.28 rather than 1.28,
well past the 1.3 benchmark. Step 2 only earns a slot when the literal spelling
is genuinely expensive, and it should hold an output of a *different kind* from
step 1 — `ng` versus `n't` is a letter cluster versus a contraction, and that
difference is itself the cue. Two rules that differ only in degree would have to
be counted, and counting presses is the failure mode this keymap avoids.

### State-machine invariants

Four rules the adaptive-key code depends on. All four were violated at some
point, and each violation ate a character or latched a key.

1. **Only a tap may arm a pending trigger.** QMK delivers a tap-hold press
   record once resolved, with `tap.count == 0` for a hold. Without that test a
   key merely *held* as a mod or layer armed its tap letter — holding `Space`
   for `_SYM` to type `->` then pressing Magic injected the word `the`.
2. **A newer keystroke invalidates a pending one.** `hd_magic_get_trigger()`
   prefers pending unconditionally, so a stale pending outranked the letter
   actually typed last for a full 220 ms: `e x Magic` gave `exs`, not `exp`.
   Cleared in the plain-key branch only — *not* in `hd_magic_push()`, which also
   runs from inside the emission helpers, where clearing would break the
   consumed-pending release check and disarm the step-2 cycle.
3. **Never `return false` from a tap-hold release.** That skips
   `process_record_handler`, which is where QMK retires the tap, so the letter
   stayed registered in the HID report forever. Skip the bookkeeping instead.
   Same defect latched `KC_F24` after *every* Repeat tap.
4. **Strip mods before the backspace, not just before the emission.**
   `tap_code()` sends the current report, so a held Ctrl made the erase
   `Ctrl+Backspace` and deleted a whole word. Use `hd_tap_bspc_unmodded()`.

**Caps Word needs explicit handling.** Its shift is a *weak* mod, and
`quantum/action.c:95-98` clears weak mods on every press before the
`process_record` chain — then `return false` skips `process_caps_word`, so
nothing re-adds it. Any path that emits a letter must consult
`is_caps_word_on()` itself. `hd_compose_send()` must additionally *clear* weak
mods: a leaked shift turned `Compose , c` into `Compose < C`, which matches
nothing, so the sequence aborted and the already-backspaced trigger was gone.

### Chained Magic is inert by design

`hd_magic_send()` pushes what it emitted as the new history key, so without a
guard a second Magic re-enters with the *completion* as its trigger. Several
completions are themselves triggers, which produced silent nonsense: `m + Magic
+ Magic` gave `mph`, `x + Magic + Magic` gave `xph`, and `c + Magic + Magic`
gave `cq` — the emitted `k` hit the replacement-style `k → q` rule and ate a
character that was already correct. A second Magic with no step-2 rule now does
nothing. `k + Magic + Magic → qu` still works, because `k → q` is
replacement-style and does not arm a step.

### Code and punctuation Magic

| Type | Gives |
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
| `~ + Magic` | `~/` |
| `$ + Magic` | `${}` with the cursor placed inside |

## Arcane — accents

`HD_ARCANE` is on the **left middle thumb**. Accents used to ride on Magic,
which forced a chain of proxy triggers (`x → ç`, `y → ñ`, `z → ß`) to dodge
collisions with the letter rules. A separate key removes every collision, so the
natural trigger works directly.

| Type | Once | Twice | Three times |
|---|---|---|---|
| `a` `e` `i` `o` `u` | `á é í ó ú` | `à è ì ò ù` | `ä ë ï ö ü` |
| `n` | `ñ` | | |
| `c` | `ç` | | |
| `s` | `ß` | | |
| `1` | `¡` | | |
| `?` | `¿` | | |
| `@` | `€` | | |
| `.` or `;` | `·` | | |

Shift on the trigger gives the capital, and **the case is latched for the whole
cycle**: `A` + Arcane + Arcane + Arcane gives `Á À Ä`, not `Á à ä`. The Shift
that capitalised the trigger is released long before the second press, so the
case cannot be re-derived per press — it is decided once, when the cycle starts.
A Shift deliberately held during a cycling press can still force upper.

The non-vowel triggers have nothing to cycle to, so a second Arcane on one of
them is a no-op rather than a re-emit.

The vowel cycle is replacement-style: it backspaces and re-sends, so it is
timer-free like the Magic cycle above.

`_LANG` remains as an explicit one-shot layer with the same characters, laid out
one row per mark. Arcane is faster day to day because it works from the letter
you already typed; the layer is there when you would rather point at it.

**Known cost:** the accent load lands on the left thumb, which pushes left-thumb
share from 4.0% to 6.5% in Spanish and 4.3% to 6.9% in Catalan. Given the
tendonitis this is the one place the current design spends left-hand budget
rather than saving it. If it becomes a problem, the alternative host is a
right-pinky vertical chord (`'` + `h`), which is deliberately left free.

## Repeat / Nav

```c
#define REP_NAV LT(_NAV, KC_F24)
```

Tap repeats the last key, hold opens `_NAV`. `_NAV` also carries `QK_AREP`
(alt-repeat) on the right inner thumb, which repeats the last key's complement
rather than the key itself.

Repeat is a private implementation (`hd_do_repeat`), not QMK's built-in, because
calling `tap_code16(QK_REP)` from a custom dual-role key does not reliably work.
`REPEAT_KEY_ENABLE` stays on for `QK_AREP` and
`get_alt_repeat_key_keycode_user`.

**Priority:** Nav quality beats perfect ultra-fast Repeat edge cases. Do not make
Nav worse to fix a rare Repeat timing issue.

## Timing

```c
TAPPING_TERM    250     PERMISSIVE_HOLD
QUICK_TAP_TERM    0     CHORDAL_HOLD
FLOW_TAP_TERM   150     COMBO_TERM 55 (per-combo overrides)
```

`QUICK_TAP_TERM 0` disables quick tap, so tapping a dual-role key and then
immediately holding it gives the hold action rather than auto-repeating the tap
— without it, `r` followed by a held `R_NUM` sprays `rrrr` instead of opening
`_NUM`. `FLOW_TAP_TERM 150` forces a tap when you are already typing fast, which
is most of what keeps the home-row mods usable.

### Word briefs — one, not a family

`Space + Magic → "the"` is the only one. It is negative on both axes (EN dMot
−0.35, dLeft −0.71pp), which almost nothing else manages, and it is the most
common word in English so there is nothing to memorise. Space alone is
untouched, so the most-pressed key on the board does not become a decision
point.

The family was rejected twice before, and only the *cycle* form deserved that.
A counted chain on space — press twice for `and`, four times for the Catalan
one — makes position arbitrary across three languages. A single rule does not.

Most short words cannot benefit at all: `de`, `no`, `la`, `el`, `es`, `en`, `un`,
`of`, `to` each cost **0.5** to type in full because the layout alternates hands
on them, while a Magic round trip from space costs **1.0**. Only words above 1.0
literal are candidates, which is why `the` (1.5) and `and` (1.5) are the whole
English shortlist.

`and` would fit on step 2 at ratio 0.75 — under the 1.3 benchmark — but it costs
a counted press, so it is deliberately not there. The hook exists:
`hd_magic_word_brief()` arms `hd_magic_cycle_key = KC_SPC`, so a step 2 would be
tested exactly the way `n't` tests `KC_N`.

**`h + Magic` was measured and rejected.** Every 1-, 2- and 3-letter
continuation of `h` was searched; the best rule that exists is `h → er` at
−0.07pp, and `have` is −0.02pp. `e → es` delivers −1.78pp, twenty-five times
more. This is the same structural fact that killed the h-digraphs: everything
following `h` is a right-hand vowel, so there is no left-hand work to remove.

## No Calm Mode, and why

Calm Mode was a toggle that silenced some of Magic. It is gone, along with
Boring Mode before it, which disabled Magic wholesale — the left middle thumb
that one occupied became Arcane.

Both guarded a system that cannot misfire. Every adaptive rule here needs an
explicit press of a dedicated key, `Magic` or `Arcane`: no timers, no
heuristics, no autocorrect. "Turn off the clever stuff" is just "don't press
the clever key."

The scenario that sounds like a case for it — pressing Magic by accident in vim
normal mode and eating a backspace — is the one that kills it. A guard you have
to arm *before* a mistake you did not know you were going to make is not a
guard. It also had no indicator, so the realistic failure was never "I wish I
had turned it on", it was "why has my keyboard stopped completing things?"

The system that genuinely can misfire is **combos**, with their 45 ms window,
and that already has `CM_TOGG` on the Sys layer — one key to the left of where
`CALM` used to sit.

## Accented characters — Compose, not Unicode

Every accented character goes out as a **Compose sequence**: three keycodes,
`Compose` then two ordinary characters, e.g. `Compose ' a` → `á`.

This replaced QMK's `UNICODE_MODE_LINUX`, which types `Ctrl+Shift+U`, then the
hex codepoint, then `Enter`, and needs an IBus daemon to intercept all of that
and hand back a single character. There is no IBus on this machine — Wayland
under Hyprland with `GTK_IM_MODULE`, `QT_IM_MODULE` and `XMODIFIERS` all unset —
so nothing caught the chord and every app received the raw keystrokes. A
terminal drops Shift when computing the control code, saw a bare `Ctrl+U`, and
ran readline's `unix-line-discard`; that is why the line disappeared. The hex
digits then arrived as literal text.

Compose is resolved by libxkbcommon **inside each client**, so it needs no
daemon and behaves the same in a terminal, a browser and an Electron app.

### The one thing it depends on

The Menu key must be mapped to `Multi_key`, via the `compose:menu` xkb option.
**Without it the firmware is not broken but the output is garbage**: the Menu
keypress does nothing (or opens a context menu, which then eats the rest), and
the two following characters land as literal text — `a` + Arcane gives `'a`.
Pressing Arcane again compounds it, because the cycle issues one backspace per
step while a failed sequence has printed two characters:

```
'a  ->  '`a  ->  '`"a  ->  ...
```

This is set in `~/nixos-config/home/dani/wm/hyprland.nix`, appended to the
option that was already there:

```nix
kb_options = "shift:both_capslock_cancel,compose:menu";
```

To try it before rebuilding:

```bash
hyprctl keyword input:kb_options "shift:both_capslock_cancel,compose:menu"
```

`COMPOSE` also sits on the Sys layer (left index, bottom row) as a literal
Compose key, for characters the firmware does not cover. It replaced `KC_APP`,
which is the same keycode — the context menu is now `Shift+F10`.

### The sequences

| | acute | grave | diaeresis | |
|---|---|---|---|---|
| a e i o u | `Compose ' a` | `` Compose ` a `` | `Compose " a` | Shift on the vowel gives the capital |

| char | sequence | | char | sequence |
|---|---|---|---|---|
| `ñ` | `Compose ~ n` | | `¡` | `Compose ! !` |
| `ç` | `Compose , c` | | `¿` | `Compose ? ?` |
| `ß` | `Compose s s` | | `€` | `Compose c =` |
| `ẞ` | `Compose S S` | | `·` | `Compose . -` |

All of these were taken from the system Compose file rather than from memory,
and they assume the **`us`** base layout — which is group 0 in
`kb_layout=us,es`. Switching to the `es` group breaks them, but it breaks every
symbol key on the board too, so it is not a new problem.

## Tools

- **`layers.html`** — all eight layers drawn in physical column order, with the
  Magic, Arcane and chord tables. Self-contained; open it in any browser.
- **`check-layers.py`** — prints every layer from `keymap.c` in physical column
  order. The `keymaps[]` arrays interleave the two halves and store the right
  half innermost-first, which is easy to mis-read by eye; run this to check a
  diagram against the firmware rather than against memory.

  ```bash
  python3 check-layers.py
  ```

## Loose ends

Things that are true right now and might not be intended:

- `SEL_WORD`, `SEL_LINE` and `KILL_LINE` are implemented in
  `process_record_user` but are not on any layer, so they are unreachable.
  `REDO`, `ALT_STAB` and `SELW_L` are defined but unused.
- `r + Magic → s` fires from either `r` key. It is only a win from the left
  thumb; from the right index-inner `r`, `r → Magic` is a same-finger bigram and
  the rule is a net loss. The firmware cannot currently tell the two apart —
  both normalise to `KC_R`.
- The right index-inner vertical chord (`Magic` + `r`) is unassigned. It is the
  obvious home for a second adaptive key, but every letter-completion candidate
  measured badly there: the chord pays its own cost *on top of* the same
  follow-through tax Magic pays, so it wants low-frequency, high-cost-per-use
  content rather than letters.
- `layers.html` is generated by hand, not from `keymap.c`. Changing the layout
  means updating it too — `check-layers.py` will tell you if they have drifted.
