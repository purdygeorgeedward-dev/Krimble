# Krimble Status Index

Read this first. One line per item — only open KRIMBLE_CHANGES.md if you
need the full explanation, reasoning, or diff for a specific fix. Update
this file whenever a bug's status changes; regenerate from
KRIMBLE_CHANGES.md if this file and the changelog ever disagree.

## Bugs
1. Krita icon instead of Krimble icon — FIXED `a4a5b53`
2. Brush presets open on load — LIKELY FIXED `a73387e`, unconfirmed on-device
3/6. UI-scale-on-startup dialog / zoom scale — FIXED (2026-09-03)
4. Contrast doesn't reach full black/white — FIXED (2026-09-03)
5. Resize handles hard to grab — OPEN, not investigated
7. Window docking broken — FIXED `c50f03a`, unconfirmed on-device
8. Transform tool does nothing — FIXED `05d1fc1`
9. Brush painting lag — OPEN, needs on-device profiling
10. Type tool does nothing — OPEN, needs on-device repro; tool disabled in KoToolRegistry for now
11. Cartoon mascot in support screen — USER-OWNED, not Claude's task
12. Docker panels render as unmanageable strips — FIXED, two contributing bugs: `c50f03a` (global setAllowedAreas contradiction) + `af46ff2` (4 dockers defaulting to DockRight). Unconfirmed on-device.
13. File dialogs wrong default size — FIXED `4016d2b`
14. Dialog size/position not persisted — FIXED `c27416b`
15-17. Convert to Profile / Shortcuts / Toolbar customization missing from Edit menu — FIXED `623f3c5`
18. Assign Profile doesn't exist — OPEN, not investigated
19. Purge missing — NOT A BUG, already exists in Image menu
20. Step Forward/Backward missing — NOT A BUG, Undo/Redo already covers it (Krita's Undo isn't a toggle)

## Standing rules
12 rules governing all Krimble work — see "Standing rules for this fork"
section in KRIMBLE_CHANGES.md. Not duplicated here to avoid two copies
drifting out of sync. Key ones worth remembering without opening that
file: no change without showing the exact diff and getting explicit
approval; no batching multiple files under one approval; every fix needs
both an inline code comment AND a KRIMBLE_CHANGES.md entry; never write
"Photoshop" in README.md/KRIMBLE_CHANGES.md (use "industry standard");
mobile-first design priority, then industry-standard parity.

## Website / Forum
- krimble.org: LIVE, images uploaded and confirmed rendering
- forum.krimble.org: IN PROGRESS — Flarum zip uploaded to
  `~/krimble-forum/`, extraction in progress as of 2026-09-10, DB
  `dcitzdte_flarum` already created with full privileges, subdomain
  Document Root not yet pointed at `public/`

## Build
- Server: Oracle instance `instance-20260819-1011`, NDK `27.3.13750724`
- Last confirmed: `libkrita_arm64-v8a.so` compiles and links clean
  (2026-09-06). No confirmed packaged APK yet — full cheat sheet at
  `claude/KRIMBLE-BUILD-CHEATSHEET.md`

Last updated: 2026-09-10
