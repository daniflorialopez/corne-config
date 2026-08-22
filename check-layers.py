#!/usr/bin/env python3
"""Print every layer of keymap.c in physical column order.

The keymaps[] arrays are stored in QMK matrix order, which interleaves the two
halves and stores the right half innermost-first. That is easy to mis-read by
eye -- this prints what each finger actually sits on, so layers.html (or any
other diagram) can be checked against the firmware rather than against memory.

    python3 check-layers.py
"""
import os, re, sys
HERE = os.path.dirname(os.path.abspath(__file__))
src = open(os.path.join(HERE, 'keymap.c')).read()
src = src[src.index('const uint16_t PROGMEM keymaps'):]
src = re.sub(r'/\*.*?\*/', '', src, flags=re.S)          # strip comments

def split_top(s):
    "split on commas not inside parens"
    out, depth, cur = [], 0, ''
    for ch in s:
        if ch == '(': depth += 1
        elif ch == ')': depth -= 1
        if ch == ',' and depth == 0:
            out.append(cur.strip()); cur = ''
        else:
            cur += ch
    if cur.strip(): out.append(cur.strip())
    return out

blocks = re.findall(r'\[(_\w+)\]\s*=\s*LAYOUT_split_3x5_3_ex2\((.*?)\n    \)', src, flags=re.S)
LEFT  = ["pinky","ring","mid","index","inner","extra"]
RIGHT = ["extra","inner","index","mid","ring","pinky"]
ok = True
for name, body in blocks:
    toks = split_top(body)
    if len(toks) != 40:
        print(f"!! {name}: {len(toks)} tokens, expected 40"); ok = False; continue
    r0, r1, r2, th = toks[0:12], toks[12:24], toks[24:34], toks[34:40]
    print(f"\n=== {name}  ({len(toks)} keys) ===")
    print("  LEFT  half, display order (outermost first):")
    for i, fn in enumerate(LEFT):
        col = [r0[i], r1[i], '—' if i == 5 else r2[i]]
        print(f"    {fn:<6} {col[0]:<26} {col[1]:<26} {col[2]}")
    print("  RIGHT half, display order (innermost first):")
    for i, fn in enumerate(RIGHT):
        col = [r0[6+i], r1[6+i], '—' if i == 0 else r2[4+i]]
        print(f"    {fn:<6} {col[0]:<26} {col[1]:<26} {col[2]}")
    print(f"  THUMBS L outer->inner: {th[0]}, {th[1]}, {th[2]}")
    print(f"         R inner->outer: {th[3]}, {th[4]}, {th[5]}")
print("\nall blocks parsed" if ok else "\nERRORS")
