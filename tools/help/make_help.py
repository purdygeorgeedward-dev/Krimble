#!/usr/bin/env python3
"""Makes krita/data/aboutdata/krimble-help.md (the help inside the app) from tools/help/help-start.md, KRIMBLE_SCRIPT.md and tools/help/help-end.md.
Run it after changing any of the three:   python3 tools/help/make_help.py
The Krimble Script part is taken from KRIMBLE_SCRIPT.md, so that the two cannot differ."""
import os, re, sys
root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
def read(p): return open(os.path.join(root, p), encoding='utf-8').read()
script = read('KRIMBLE_SCRIPT.md')
# no "for the developers" part in the help
script = script.split('\n## For the developers')[0].rstrip('\n') + '\n'
lines = script.split('\n')
out = []
for l in lines:
    # a table is hard to read on a phone: each row of the table of commands becomes one line, "- command: what it does"
    m = re.match(r'^\|\s*(.+?)\s*\|\s*(.+?)\s*\|\s*$', l)
    if m:
        if m.group(1) in ('Command',) or set(m.group(1)) <= set('-: '):
            continue                 # the head of the table and the line under it
        out.append('- ' + m.group(1) + ': ' + m.group(2))
        continue
    if l.startswith('# '):          # "# Krimble Script" becomes a part of the help
        out.append('## ' + l[2:])
    elif l.startswith('## '):       # its parts become sub-parts
        out.append('### ' + l[3:])
    else:
        out.append(l)
text = read('tools/help/help-start.md').rstrip('\n') + '\n\n' + '\n'.join(out).rstrip('\n') + '\n' + read('tools/help/help-end.md')
# a link to a repo file means nothing inside the app
text = text.replace('See [KRIMBLE_SCRIPT.md](KRIMBLE_SCRIPT.md)', 'See Krimble Script')
dest = os.path.join(root, 'krita', 'data', 'aboutdata', 'krimble-help.md')
open(dest, 'w', encoding='utf-8').write(text)
print('wrote', os.path.relpath(dest, root), len(text), 'bytes,', text.count('\n## '), 'parts')
