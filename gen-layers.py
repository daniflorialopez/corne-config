#!/usr/bin/env python3
"""Generate layers.html from keymap.c.

    python3 gen-layers.py            # rewrite layers.html
    python3 gen-layers.py --check    # exit 1 if layers.html is stale

layers.html used to be maintained by hand, and it drifted every time a rule
changed -- four Magic rules were added in one session and the diagram still
showed the old twelve. Everything that can go stale is now derived from
keymap.c: the key grids, the Magic table, the Arcane table and the combos.

What stays hand-written is the prose that cannot be derived -- layer names,
the one-line "how you get here", the side notes, and the per-rule annotations
in MAGIC_NOTES / COMBO_NOTES below. A rule with no annotation still appears in
the output with an empty note, so a new rule can never silently vanish from the
diagram; the worst that happens is it shows up undocumented.

The shell (CSS, markup, rendering JS) lives in layers.template.html.
"""
import os
import re
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
KEYMAP = os.path.join(HERE, 'keymap.c')
TEMPLATE = os.path.join(HERE, 'layers.template.html')
OUTPUT = os.path.join(HERE, 'layers.html')

# Sentinels for the transparent and blank keys. Deliberately NOT the strings
# "T" and "X": those are themselves key labels, and comparing by value made
# KC_T on QWERTY render as transparent and KC_X on Base render as blank.
T = '\x00trans'
X = '\x00none'

# --------------------------------------------------------------------------
# Editorial: everything below here is written by a human, not derived.
# --------------------------------------------------------------------------

LAYER_META = {
    '_BASE': dict(
        id='base', name='Base — Vibranium-f', how='default layer',
        chords={
            'v': [dict(col=2, seam=0, out='x'), dict(col=2, seam=1, out='f'),
                  dict(col=3, seam=0, out='k'), dict(col=3, seam=1, out='v'),
                  dict(col=4, seam=0, out='j'), dict(col=4, seam=1, out='s')],
            'h': [dict(row=0, out='z'), dict(row=1, out='q')],
        },
        notes=[
            "Home rows are <code>S C N T</code> and <code>A E I H</code>; hold any of them for Alt Ctrl Gui Shift. Chordal Hold plus Flow Tap keep them out of the way during fast same-hand runs.",
            "The index finger covers three columns on each hand — index, inner and extra. Only <code>h</code> sits on a pinky home key, on the right.",
            "Two layer-taps survive on letters: <code>O</code> for Num on the right hand, <code>R</code> on the left inner thumb. Every other subhome hold was removed — a mistimed hold on a letter is the misfire that costs most.",
        ]),
    '_NAV': dict(
        id='nav', name='Nav / Edit', how='hold Repeat — right inner thumb',
        notes=[
            "Arrows under the right home row, page and word motions flanking them. The left hand mirrors the home-row mods as plain modifiers, so you can hold Shift or Ctrl while navigating.",
            "The right inner thumb becomes Alt-Repeat: it repeats the last key's complement rather than the key itself.",
        ]),
    '_SYM': dict(
        id='sym', name='Symbols', how='hold Space — right middle thumb',
        notes=[
            "Shifted number row on the left. Brackets in matched pairs on the right: open on the home row, close on the bottom row directly below it.",
            "<code>q</code> and <code>z</code> live here as a fallback — the fast way to reach them is the <code>T+K</code> and <code>J+G</code> chords.",
        ]),
    '_NUM': dict(
        id='num', name='Numbers / Function', how='hold R — left inner thumb — or hold O',
        notes=[
            "Numpad under the right hand, F1&ndash;F15 under the left. <code>F13 F14 F15</code> replaced the old duplicate <code>0 . =</code>: they are unbound in most desktops, which makes them clean targets for window-manager and app shortcuts.",
            "Two ways in, on purpose. Left thumb when the right hand is typing digits; the right-hand <code>O</code> hold when the left hand is on the F-keys.",
        ]),
    '_LANG': dict(
        id='lang', name='Language / Compose', how='tap Lang — left extra column, top',
        notes=[
            "A one-shot layer: tap it, then tap the character. This is the explicit route — day to day the Arcane thumb key is faster, because it works from the letter you already typed.",
            "Laid out by mark, one row each: acute, grave, diaeresis.",
        ]),
    '_SYS': dict(
        id='sys', name='System', how='hold Sys — either extra column, home row',
        notes=[
            "Held from either half, so it is reachable whichever hand is free. Media and brightness on the left, transport on the right.",
            "<code>Vibra</code> and <code>QWERTY</code> switch the persistent default layer. <code>Boot</code> sits on both extra columns and is the only destructive key on the board.",
        ]),
    '_MOUSE': dict(
        id='mouse', name='Mouse / Scroll', how='toggle from Sys',
        notes=[
            "A toggle, not a hold — you are usually in it for a while. The same key that entered it, left inner column top, leaves it.",
            "Pointer on the right home row, wheel above, buttons below. Everything else falls through, so modifiers and Space still work.",
        ]),
    '_QWERTY': dict(
        id='qwerty', name='QWERTY rescue', how='Sys → QWERTY — persistent',
        notes=[
            "For handing the keyboard to somebody else, or for software that reads raw scancodes and would be confused by Vibranium-f.",
            "No home-row mods, no Magic, no chords — deliberately boring. Nav and Sym move to the thumbs so it still behaves like a 40-key board.",
        ]),
}

# (trigger, output) -> (EN frequency, note).  Missing entries render blank.
MAGIC_NOTES = {
    ('e', 'es'):    ('0.990%', 'biggest relief on the board — about 25% of left-pinky load'),
    ('n', 'ng'):    ('0.631%', 'largest single-letter reduction in left-hand keystrokes'),
    ('Space', 'the'): ('0.705%', 'word brief — saves motion AND left-hand load'),
    ('i', 'is'):    ('0.404%', 'all of the relief lands on the left pinky'),
    ('s', 'sta'):   ('0.313%', 'free lunch — the only rule that emits two letters'),
    ('r', 'rs'):    ('0.298%', 'left-pinky relief; costs a little motion'),
    ('t', 'ts'):    ('0.281%', 'replaced t→h, which cost more than every other rule earned'),
    ('c', 'ck'):    ('0.158%', 'replaced c→h'),
    ('m', 'mp'):    ('0.154%', 'best-scoring rule in all three languages'),
    ('w', 'wh'):    ('0.137%', 'kept — near-free, and long-standing muscle memory'),
    ('g', 'gh'):    ('0.129%', 'kept — the cheapest of the six h-digraphs'),
    ('l', 'ls'):    ('0.120%', 'carried by Catalan, where it is far more frequent'),
    ('p', 'ph'):    ('0.080%', 'kept'),
    ('x', 'xp'):    ('0.037%', 'x sits on the worst key of the injured hand'),
    ('d', 'dv'):    ('0.035%', 'advance, advice, advantage'),
    ('q', 'qu'):    ('—', 'q itself comes from the T+K chord'),
    ('k', 'q'):     ('—', 'replacement, not append: keeps q off the symbol layer'),
}

MAGIC_EXTRA_ROWS = [
    ('n, then Magic again', "n't", '—', "second press rewrites ng into n't"),
    ('anything, then Magic again', '—', '—',
     'inert: a chained Magic has no rule and does nothing'),
]

# combo array name -> note
COMBO_NOTES = {
    'k_combo':  'harmless payload: Esc-then-a is the vim sequence most likely to misfire here',
    'f_combo':  'au is same-finger, so it cannot roll into the chord',
    'x_combo':  'x deletes a char in vim, so it goes on the chord you never type by accident',
    'v_combo':  '',
    'j_combo':  'top+home, not top+bottom — no two-row span',
    's_combo':  'heaviest chord letter on the least-loaded right finger',
    'z_combo':  'horizontal, index and inner columns, top row',
    'qu_combo': 'horizontal, home row — then q + Magic for qu',
}

# --------------------------------------------------------------------------
# Keycode -> display label.  "TOP|SUB|roles"; roles: mod layer adapt trans none warn
# --------------------------------------------------------------------------

LABELS = {
    '_______': T, 'XXXXXXX': X,

    'HRM_S': 'S|alt|mod',   'HRM_C': 'C|ctrl|mod', 'HRM_N': 'N|gui|mod',
    'HRM_T': 'T|shift|mod', 'HRM_A': 'A|shift|mod', 'HRM_E': 'E|gui|mod',
    'HRM_I': 'I|ctrl|mod',  'HRM_H': 'H|alt|mod',

    'HD_MAGIC': 'Magic|adaptive|adapt', 'HD_ARCANE': 'Arcane|accents|adapt',
    'REP_NAV': 'Repeat|nav|adapt', 'QK_AREP': 'AltRep|adaptive|adapt',
    'SPC_SYM': 'Space|sym|layer', 'R_NUM': 'R|num|layer', 'O_NUM': 'O|num|layer',
    'LANG': 'Lang|osl|layer', 'SYS': 'Sys|hold|layer',
    'NAV': 'Nav|hold|layer', 'SYM': 'Sym|hold|layer',
    'CW_TOGG': 'Caps|word', 'MOUSE_TG': 'Mouse|toggle',
    'CM_TOGG': 'Combo|toggle', 'SH_TOGG': 'Swap|hands',
    'VIBRA': 'Vibra|default', 'QWERTY': 'QWERTY|default',
    'QK_BOOT': 'Boot|dfu|warn', 'COMPOSE': 'Compose',

    'P_DOT_SLSH': '. /', 'P_COMM_QUES': ', ?', 'P_QUOT_DQUO': '\' "',

    'KC_BSPC': '⌫|bspc', 'KC_ENT': '↵|enter', 'KC_ESC': 'Esc',
    'KC_TAB': 'Tab', 'KC_SPC': 'Space', 'KC_DEL': 'Del', 'KC_INS': 'Ins',
    'KC_NUM': 'NumLk', 'KC_PSCR': 'PrtSc', 'KC_SCRL': 'ScrLk', 'KC_PAUS': 'Pause',

    'KC_LALT': 'Alt', 'KC_LCTL': 'Ctrl', 'KC_LGUI': 'Gui', 'KC_LSFT': 'Shift',

    'KC_UP': '↑', 'KC_DOWN': '↓', 'KC_LEFT': '←', 'KC_RGHT': '→',
    'KC_PGUP': 'PgUp', 'KC_PGDN': 'PgDn', 'KC_HOME': 'Home', 'KC_END': 'End',

    'UNDO': 'Undo', 'CUT': 'Cut', 'COPY': 'Copy', 'PASTE': 'Paste',
    'SAVE': 'Save', 'FIND': 'Find', 'S_TAB': '⇧Tab',
    'PREVT': 'Prev|tab', 'NEXTT': 'Next|tab', 'ALT_TAB': 'Alt|tab',
    'WORD_L': '← word', 'WORD_R': 'word →', 'C_BSPC': 'Ctrl|⌫', 'C_DEL': 'Ctrl|del',

    'KC_MUTE': 'Mute', 'KC_VOLD': 'Vol -', 'KC_VOLU': 'Vol +',
    'KC_BRID': 'Bright -', 'KC_BRIU': 'Bright +',
    'KC_MPRV': '⏮', 'KC_MPLY': '⏯', 'KC_MNXT': '⏭', 'KC_MSTP': '⏹',

    'MS_BTN1': 'L click', 'MS_BTN2': 'R click', 'MS_BTN3': 'M click',
    'MS_UP': '↑', 'MS_DOWN': '↓', 'MS_LEFT': '←', 'MS_RGHT': '→',
    'MS_WHLU': 'Wh ↑', 'MS_WHLD': 'Wh ↓', 'MS_WHLL': 'Wh ←', 'MS_WHLR': 'Wh →',

    'KC_MINS': '-', 'KC_EQL': '=', 'KC_LBRC': '[', 'KC_RBRC': ']',
    'KC_BSLS': '\\', 'KC_SCLN': ';', 'KC_COMM': ',', 'KC_DOT': '.',
    'KC_SLSH': '/', 'KC_GRV': '`', 'KC_QUOT': "'",
}

SHIFTED = {
    'KC_1': '!', 'KC_2': '@', 'KC_3': '#', 'KC_4': '$', 'KC_5': '%',
    'KC_6': '^', 'KC_7': '&', 'KC_8': '*', 'KC_9': '(', 'KC_0': ')',
    'KC_MINS': '_', 'KC_EQL': '+', 'KC_LBRC': '{', 'KC_RBRC': '}',
    'KC_BSLS': '|', 'KC_SCLN': ':', 'KC_COMM': '<', 'KC_DOT': '>',
    'KC_GRV': '~', 'KC_SLSH': '?', 'KC_QUOT': '"',
}

# XC_* Compose keycodes -> the character they produce.
XC_CHAR = {
    'XC_AACU': 'á', 'XC_EACU': 'é', 'XC_IACU': 'í', 'XC_OACU': 'ó', 'XC_UACU': 'ú',
    'XC_AGRV': 'à', 'XC_EGRV': 'è', 'XC_IGRV': 'ì', 'XC_OGRV': 'ò', 'XC_UGRV': 'ù',
    'XC_AUML': 'ä', 'XC_EUML': 'ë', 'XC_IUML': 'ï', 'XC_OUML': 'ö', 'XC_UUML': 'ü',
    'XC_ENYE': 'ñ', 'XC_CCED': 'ç', 'XC_ESZ': 'ß',
    'XC_IEXC': '¡', 'XC_IQUE': '¿', 'XC_EURO': '€', 'XC_MDOT': '·',
}

KC_NAME = {'KC_SPC': 'Space', 'KC_DOT': '.', 'KC_SCLN': ';', 'KC_SLSH': '/',
           'KC_COMM': ',', 'KC_1': '1', 'KC_2': '2'}


def label(tok):
    if tok in LABELS:
        return LABELS[tok]
    if tok in XC_CHAR:
        return XC_CHAR[tok]
    m = re.fullmatch(r'S\((KC_\w+)\)', tok)
    if m:
        base = m.group(1)
        if base in SHIFTED:
            return SHIFTED[base]
        raise KeyError(f'no shifted label for {tok}')
    m = re.fullmatch(r'KC_([A-Z])', tok)
    if m:
        return m.group(1)
    m = re.fullmatch(r'KC_(\d)', tok)
    if m:
        return m.group(1)
    m = re.fullmatch(r'KC_(F\d{1,2})', tok)
    if m:
        return m.group(1)
    raise KeyError(f'no label for keycode {tok!r} -- add it to LABELS in gen-layers.py')


# --------------------------------------------------------------------------
# Parsing keymap.c
# --------------------------------------------------------------------------

def strip_comments(s):
    s = re.sub(r'/\*.*?\*/', '', s, flags=re.S)
    return re.sub(r'//[^\n]*', '', s)


def split_top(s):
    out, depth, cur = [], 0, ''
    for ch in s:
        if ch == '(':
            depth += 1
        elif ch == ')':
            depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur.strip())
            cur = ''
        else:
            cur += ch
    if cur.strip():
        out.append(cur.strip())
    return out


def parse_layers(src):
    body = src[src.index('const uint16_t PROGMEM keymaps'):]
    body = strip_comments(body)
    blocks = re.findall(
        r'\[(_\w+)\]\s*=\s*LAYOUT_split_3x5_3_ex2\((.*?)\n    \)', body, flags=re.S)
    if not blocks:
        sys.exit('gen-layers: found no LAYOUT blocks')
    out = []
    for name, raw in blocks:
        toks = split_top(raw)
        if len(toks) != 40:
            sys.exit(f'gen-layers: {name} has {len(toks)} keys, expected 40')
        rows = [[label(t) for t in toks[0:12]],
                [label(t) for t in toks[12:24]],
                [label(t) for t in toks[24:34]],
                [label(t) for t in toks[34:40]]]
        out.append((name, rows))
    return out


def parse_magic(src):
    """Every Magic rule, in one list of (trigger, output)."""
    rules = []

    two = re.search(r'hd_magic_completion2\(.*?\{(.*?)\n\}', src, flags=re.S)
    if two:
        for trig, a, b in re.findall(
                r'case (KC_\w+):\s*\*first\s*=\s*(KC_\w+);\s*\*second\s*=\s*(KC_\w+);',
                two.group(1)):
            t = label(trig).lower()
            rules.append((t, t + label(a).lower() + label(b).lower()))

    one = re.search(r'static uint16_t hd_magic_completion\(.*?\{(.*?)\n\}', src, flags=re.S)
    if not one:
        sys.exit('gen-layers: could not find hd_magic_completion()')
    for trig, out in re.findall(r'case (KC_\w+):\s*return (KC_\w+);', one.group(1)):
        t = label(trig).lower()
        rules.append((t, t + label(out).lower()))

    if 'hd_magic_word_brief' in src:
        brief = re.search(r'hd_send_string_unshifted\(is_caps_word_on\(\)\s*\?\s*"[A-Z]+"\s*:\s*"(\w+)"\)', src)
        if brief:
            rules.append(('Space', brief.group(1)))

    if re.search(r'trigger == KC_K', src):
        rules.append(('k', 'q'))

    order = {r: i for i, r in enumerate(MAGIC_NOTES)}
    rules.sort(key=lambda r: order.get(r, len(order)))
    return rules


def parse_arcane(src):
    """Arcane trigger -> up to three cycle steps."""
    vowels = [('a', 'á', 'à', 'ä'), ('e', 'é', 'è', 'ë'), ('i', 'í', 'ì', 'ï'),
              ('o', 'ó', 'ò', 'ö'), ('u', 'ú', 'ù', 'ü')]
    body = re.search(r'static bool hd_do_arcane\(void\)\s*\{(.*?)\n\}', src, flags=re.S)
    if not body:
        sys.exit('gen-layers: could not find hd_do_arcane()')
    rows = [list(v) for v in vowels]
    seen = {v[0] for v in vowels}

    def key_for(trig):
        trig = trig.strip()
        m = re.fullmatch(r'S\((KC_\w+)\)', trig)
        if m:
            return SHIFTED[m.group(1)]
        return KC_NAME.get(trig, label(trig).lower())

    # Case labels stack: KC_SCLN and KC_DOT both fall through to the middot
    # return, so collect every label since the last return, not just the
    # nearest one.
    pending = []
    for line in body.group(1).splitlines():
        cm = re.match(r'\s*case ([^:]+):\s*$', line)
        if cm:
            pending.append(cm.group(1))
            continue
        rm = re.search(r'HD_SEQ\((XC_\w+)\)', line)
        if rm:
            keys = [key_for(t) for t in pending if key_for(t) not in seen]
            if keys:
                seen.update(keys)
                rows.append([' or '.join(keys), XC_CHAR[rm.group(1)], '—', '—'])
            pending = []
        elif line.strip().startswith('return'):
            pending = []
    return rows


def parse_combos(src):
    arrays = dict(re.findall(
        r'const uint16_t PROGMEM (\w+)\[\]\s*=\s*\{([^}]*?)COMBO_END', src))
    outputs = dict((m.group(2), m.group(1)) for m in re.finditer(
        r'\[(C_\w+)\]\s*=\s*COMBO\((\w+),\s*(KC_\w+)\)', src))
    # re-read properly: index -> (array, output)
    pairs = re.findall(r'\[(C_\w+)\]\s*=\s*COMBO\((\w+),\s*(KC_\w+)\)', src)
    terms = {}
    term_body = re.search(r'uint16_t get_combo_term\(.*?\{(.*?)\n\}', src, flags=re.S)
    if term_body:
        cur = []
        for line in term_body.group(1).splitlines():
            cm = re.search(r'case (C_\w+):', line)
            if cm:
                cur.append(cm.group(1))
            rm = re.search(r'return (\d+);', line)
            if rm:
                for c in cur:
                    terms[c] = rm.group(1) + ' ms'
                cur = []
    finger = {'k_combo': 'right index', 'f_combo': 'right index',
              'x_combo': 'right middle', 'v_combo': 'right middle',
              'j_combo': 'right ring', 's_combo': 'right ring',
              'z_combo': 'left index', 'qu_combo': 'left index'}
    rows = []
    for idx, arr, out in pairs:
        keys = [k.strip() for k in arrays[arr].split(',') if k.strip()]
        # Only the face of the key, not its "A|shift|mod" hold annotation.
        rows.append([' + '.join(label(k).split('|')[0] for k in keys),
                     label(out).lower(),
                     finger.get(arr, ''),
                     terms.get(idx, ''),
                     COMBO_NOTES.get(arr, '')])
    rows.sort(key=lambda r: (r[2].startswith('left'), r[2]))
    return rows


# --------------------------------------------------------------------------
# Emitting
# --------------------------------------------------------------------------

def js(s):
    return '"' + s.replace('\\', '\\\\').replace('"', '\\"') + '"'


def row_js(row):
    cells = ['T' if c == T else 'X' if c == X else js(c) for c in row]
    return '[' + ','.join(cells) + ']'


def emit_layers(layers):
    out = []
    for n, (name, rows) in enumerate(layers):
        meta = LAYER_META.get(name)
        if meta is None:
            sys.exit(f'gen-layers: no LAYER_META entry for {name}')
        parts = [f'{{ n:{n}, id:{js(meta["id"])}, name:{js(meta["name"])}, how:{js(meta["how"])},']
        if 'chords' in meta:
            v = ', '.join('{col:%d,seam:%d,out:%s}' % (c['col'], c['seam'], js(c['out']))
                          for c in meta['chords']['v'])
            h = ', '.join('{row:%d,out:%s}' % (c['row'], js(c['out']))
                          for c in meta['chords']['h'])
            parts.append('  chords:{')
            parts.append(f'    v:[ {v} ],')
            parts.append(f'    h:[ {h} ]')
            parts.append('  },')
        parts.append('  notes:[')
        parts.append(',\n'.join('    ' + js(x) for x in meta['notes']))
        parts.append('  ],')
        parts.append('  rows:[')
        parts.append(',\n'.join('    ' + row_js(r) for r in rows))
        parts.append('  ]}')
        out.append('\n'.join(parts))
    return 'const LAYERS = [\n' + ',\n\n'.join(out) + '\n];'


def emit_magic(rules):
    lines = []
    for trig, out in rules:
        freq, note = MAGIC_NOTES.get((trig, out), ('—', ''))
        lines.append(f'  [{js(trig)},{js(out)},{js(freq)},{js(note)}]')
    for r in MAGIC_EXTRA_ROWS:
        lines.append('  [' + ','.join(js(x) for x in r) + ']')
    return 'const MAGIC = [\n' + ',\n'.join(lines) + '\n];'


def emit_arc(rows):
    return ('const ARC = [\n'
            + ',\n'.join('  [' + ','.join(js(c) for c in r) + ']' for r in rows)
            + '\n];')


def emit_cb(rows):
    return ('const CB = [\n'
            + ',\n'.join('  [' + ','.join(js(c) for c in r) + ']' for r in rows)
            + '\n];')


def main():
    src = open(KEYMAP, encoding='utf-8').read()
    tpl = open(TEMPLATE, encoding='utf-8').read()

    layers = parse_layers(src)
    data = {
        '/*{{LAYERS}}*/': emit_layers(layers),
        '/*{{MAGIC}}*/': emit_magic(parse_magic(src)),
        '/*{{ARC}}*/': emit_arc(parse_arcane(src)),
        '/*{{CB}}*/': emit_cb(parse_combos(src)),
    }
    html = tpl
    for k, v in data.items():
        if k not in html:
            sys.exit(f'gen-layers: placeholder {k} missing from the template')
        html = html.replace(k, v)

    banner = ('<!-- GENERATED by gen-layers.py from keymap.c. Do not edit by hand;\n'
              '     edit keymap.c, or the prose in gen-layers.py, then re-run it. -->\n')
    html = banner + html

    if '--check' in sys.argv:
        current = open(OUTPUT, encoding='utf-8').read() if os.path.exists(OUTPUT) else ''
        if current != html:
            print('layers.html is STALE -- run: python3 gen-layers.py')
            return 1
        print('layers.html is up to date')
        return 0

    open(OUTPUT, 'w', encoding='utf-8').write(html)
    magic = parse_magic(src)
    undocumented = [f'{t}+Magic' for t, o in magic if (t, o) not in MAGIC_NOTES]
    print(f'layers.html: {len(layers)} layers, {len(magic)} Magic rules, '
          f'{len(parse_arcane(src))} Arcane rows, {len(parse_combos(src))} combos')
    if undocumented:
        print('  note: no annotation yet for ' + ', '.join(undocumented))
    return 0


if __name__ == '__main__':
    sys.exit(main())
