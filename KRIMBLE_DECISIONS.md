# Krimble decisions

Krimble 2026-11-13. George: "I'm concerned about his decisions get made and then later ignored." One line per decision: what, when, how it is checked. **Read this first** (SESSION_CATCHUP.md points here). A new decision is added here in the same push as the change it causes. "Not automatic" means only a person (or Claude) reading this file keeps it.

| Decision | Since | Checked by |
|---|---|---|
| Tag `v1.0.N-beta`, release title `Krimble v1.0.N-beta`, notes heading `# Krimble v1.0.N-beta`; the APK file name (made by the workflow) has no v | tags 2026-10-24, titles 2026-11-13 | `tools/ci/check_release_names.py` (a warning on every build) |
| Every build gets a new number; a build does not have to be released | 2026-11-08 | `tools/ci/check_build_number.py` (stops the build) |
| Every menu, form and resource file is parsed as XML before a build | 2026-11-08 | `tools/ci/check_xml.py` (stops the build) |
| The default branch is `master`; never create `main` | Bible | not automatic |
| No "Photoshop" in README.md or KRIMBLE_CHANGES.md (say "industry standard"); code comments may | Bible | `tools/ci/check_decisions.py` (a warning) |
| JNI class paths are `org/krimble/android` | 2026-09-19 | `tools/ci/check_decisions.py` (a warning) |
| Code is commented out, never deleted; menu and action XML entries are commented out | Bible | not automatic |
| Every change: an inline comment, a KRIMBLE_CHANGES.md entry, a KRIMBLE_WORKLOG.md entry, times in MST; a change is called done only when it is on the repository (`git ls-remote`), and the entry is written only after the edit is seen in the file | Bible, 2026-11-13 | not automatic |
| Dodge, Burn, Sponge, Clone Stamp and Pattern Stamp are dropped | 2026-10-20 | not automatic |
| New tools: only the Hexagon selection and the Regular Polygon and Star | 2026-11-12 | not automatic |
| Hexagon tools: one shared lock (grid, size, orientation, line thickness) until unlocked; snapping follows the hex grid of View > Show > Hex | 2026-11-12 | not automatic |
| Toolbox: starts docked at the left with two columns; it does not start floating | 2026-11-13 | not automatic |
| Toolbox flyouts: the toolbox's own icon size, next to the tool that was held | 2026-11-13 | not automatic |
| Default theme Krimble Blue-Gray: one hue family (hue 227, saturation 48, palette and style sheet), one-pixel light and dark lines, the menu bar is NOT painted (only Krimble Painted paints it), list selection is the flat bright blue `#1d7cb2` | 2026-11-13 | not automatic |
| Color wells (Color Lab and its pages): no outline, no glow | 2026-11-13 | not automatic |
| Never tell George which build he is running, that a change is not in a build yet, or that something is unverified or untested on the phone; he knows. Say it only when he asks (the facts stay in KRIMBLE_CHANGES.md) | 2026-11-09 and 2026-11-13 ("STOP TELLING ME WHICH BUILD I AM RUNNING") | not automatic |
| Color Lab must be a real panel that reflows, not a picture that zooms (the rebuild is not started) | 2026-11-13 | not automatic |
| SmartCompression: the selection is the area to compress; the film covers the protected area; editing starts from the Smart Selection; the label is "SmartCompression" (no %); logo 54 px; no explanations in the JPEG tab | 2026-11-13 | not automatic |

## Needs George's answer (two older rules disagree with later practice)

1. **Approval before every change** (the Bible, Sept 28: show the exact diff and wait for a yes, one file at a time). The work of Nov 12 and 13 has not followed it: changes were made and pushed on his request. Keep it, or retire it?
2. **Publish the current build before the number changes** (SESSION_CATCHUP.md, Oct 24) against **"We don't always have to release a new build, but we should always change the number"** (Nov 8). The later one is in the table above; the older rule is marked as replaced in SESSION_CATCHUP.md unless he says otherwise.
