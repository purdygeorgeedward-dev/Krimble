#!/usr/bin/env python3
"""Checks that every `native` method of the Java classes in packaging/android/apk/src/org/krimble/android has a C++ function with the matching JNI name
(Java_org_krimble_android_<Class>_<method>). A missing one crashes the app (UnsatisfiedLinkError) the first time Java calls it. Run before pushing Android changes:
    python3 tools/check-jni-names.py"""
import re, os, sys, glob
root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
java_dir = os.path.join(root, 'packaging/android/apk/src/org/krimble/android')
natives = []
for f in sorted(glob.glob(java_dir + '/*.java')):
    cls = os.path.basename(f)[:-5]
    for m in re.finditer(r'\bnative\s+[\w<>\[\]]+\s+(\w+)\s*\(', open(f, encoding='utf-8', errors='ignore').read()):
        natives.append((cls, m.group(1)))
cpp = ''
for dirpath, dirs, files in os.walk(root):
    dirs[:] = [d for d in dirs if d not in ('.git', '3rdparty', 'translations')]
    for fn in files:
        if fn.endswith(('.cpp', '.cc', '.h', '.c')):
            try: cpp += open(os.path.join(dirpath, fn), encoding='utf-8', errors='ignore').read()
            except OSError: pass
# a name that appears only inside a comment ("// was: Java_org_krita...") does not count: look for real definitions
bad = 0
for cls, method in natives:
    name = 'Java_org_krimble_android_%s_%s' % (cls, method)
    defined = re.search(r'^[^/\n]*\b' + name + r'\s*\(', cpp, re.M) is not None
    print(('ok      ' if defined else 'MISSING ') + name)
    bad += 0 if defined else 1
print('%d native methods, %d missing' % (len(natives), bad))
sys.exit(1 if bad else 0)
