# Krimble — Session Catch-Up

(Rewritten 2026-10-24, about 5:30 AM Mountain Time, for a new chat inside the Krimble project. The older Sept 19–20 version was out of date and is replaced; the repo docs listed below are the source of truth.)

I'm **George** (George Edward Purdy), building **Krimble**, a professional Android graphics app (Photoshop-caliber — never call it a "paint app") forked from Krita. Repo: `github.com/purdygeorgeedward-dev/krimble`, branch `master`. I'm in El Paso (Mountain Time); I work from my phone.

## How to work with me
- Terse, direct. No padding, no affirmations, no empathy statements. Don't say "Understood", "frustrating", or "fair" (say "okay").
- **Answer what I ask. Put suggestions and next steps as offers, not orders** (I said so on 2026-10-24).
- Vision issue: large, clear formatting; no dense paragraphs. Ask for missing information instead of guessing.
- **Commands go in their own message block, separate from talk, labelled phone (Termux) or cloud server.** For the server: first a separate log-in step, then the commands to run inside that session. No one-line `ssh ... "command"`. No multiple-choice option lists or tappable choice buttons.
- Comment out code, don't delete it. Never say a change is done unless it is in the repo. **Make dated notes of every GitHub push** (a numbered entry in `KRIMBLE_CHANGES.md`).
- Check the date and time at the start of a session. Only pursue things that lead to a functional, marketable build.
- I can't use social media and have nobody to promote my work for me.

## Where things are
- Source repo on the server: `~/krimble` (git). Build tree: `~/kwd/krita` (`_build`, `_install`, `_packaging`). Server: Oracle Cloud `instance-20260819-1011`, `129.151.24.131`, user `ubuntu`, key `~/.ssh/ssh-key-2026-08-19.key` on my phone. Builds are free on this server (no paid services). Android NDK for builds: keep r27 (`27.3.13750724`).
- Server scripts (`tools/server/`, copied to `~/`): `kb-build.sh` (pulls and compiles, about 2 hours), `kb-package.sh` (APK, about 10 min), `kb-status.sh` (can show stale results), `kb-apk.sh` + `kb-release.sh --publish` (attach the APK to the GitHub release; asks for my GitHub token).
- Docs in the repo: `BUILD_ANDROID.md` (build and release process, AAB for Google Play), `KRIMBLE_CHANGES.md` (every change, numbered; read the last entries first), `KRIMBLE_TEST_LIST_2026-10-23.md` (my test list with status and CLAIMED marks), `KRIMBLE_ROADMAP.md`, `KRIMBLE_STATUS.md`, `KRIMBLE_PHASE2_EFFECTS.md` (effects pack plan), `KRIMBLE_RESEARCH_storytold.md` (notes on github.com/storytold, PhotoCraft).
- Push method: I paste a GitHub PAT into the chat when a push is needed (never stored). Commit identity: George Purdy / purdygeorgeedward@gmail.com. The working clone is a **partial clone**: use `git read-tree origin/master`, `git add` **exact file paths only (never a directory)**, check `git diff --cached --name-status origin/master` shows **no D lines**, `git commit-tree`, push without force. (On 2026-10-24 a `git add krita` deleted about 1,785 files for a minute; fixed in `4c12bde`.)
- **Another chat may be working the same list** (its commits are authored `purdygeorgeedward-dev`). Before an item, check the test list file for CLAIMED marks; claim by editing it.

## Process rules (mine)
1. **The current build must be published on GitHub (tag, notes, known issues, APK attached) before the version number is changed or a new build is started.** `BUILD_ANDROID.md` section 7, step 0. Check the Releases list first. Packaging replaces the APK in `_packaging`, so release before packaging.
2. **Tag format: `v1.0.N-beta`** (e.g. `v1.0.42-beta`). All releases 29 to 42 use it. Release title "Krimble 1.0.N beta", pre-release, notes with "NEW IN" and "KNOWN ISSUES".
3. A GitHub release for a build is created as a draft through the API (target = the commit that set that version's number), then published on the server with `~/kb-apk.sh` and `~/kb-release.sh --publish`.

## State as of 2026-10-24, about 5:30 AM MT
- **Published:** 1.0.42 (`v1.0.42-beta`, commit `f0c70d9`). Version number in the repo is already **43** (set in `235e92d`).
- **Building now:** 1.0.43, started 5:00 AM MT on the server from `45cb4b4` (the head of `master` at that time). Compile takes about 2 hours. After the compile: `~/kb-package.sh`, then draft release `v1.0.43-beta`, then `~/kb-apk.sh` and `~/kb-release.sh --publish`; **only after that** a fresh AAB (`BUILD_ANDROID.md` section 5; it wipes `_packaging`; versionCode must be new, 43 gives 5050443).
- **Not yet tested on a phone (all in 43):** Smudge fix (the real cause: `KoToolManager` renames tools to their action id, so the old name checks never matched), Stack Panels via remove/add, ring-by-ring Content-Aware Fill start (tested on the bench only), toolbox no longer flipping to horizontal while dragged, About > Also Thanks To with the README's thank-you list and logos, selection tools starting on Replace, Reset Tool Options on Start, File > Revert, windows that scroll instead of being wider than the screen.
- **Findings from my 1.0.42 test, fixed in 43 but unconfirmed:** Smudge took over other tools; Stack Panels put panels side by side; New window icons giant (the 3D pictures were taken out again; the Preferences 3D icons stay); toolbox stuck horizontal while dragged; Free Transform corner arrows too big; right panels reluctant to attach; edge tab twice as tall; no tab on the right panel (the layout-info overlay now says why a tab is hidden).
- **Open:** test list items 10 Customize Toolbar, 11 Preferences icon size, 13 Edit Presets (more presets, 256-colour palettes; see the PhotoCraft notes), 14 Auto Color (a vibrance-style default; Photoshop's own is levels plus midtone neutralising), 22 Quick Mask, 25 Text window redesign, 26 Text menu; questions: 8 Sample Color, 12 Color Mode. Type tool: emoji invisible, restyle part of text. Hand tool once moved the layer. Content-Aware Fill on busy pictures still needs the AI add-on later.
- **Play Store plan:** no listing yet. A new personal account needs a closed test with at least 12 testers opted in for 14 continuous days, then an application for production access (about 3 weeks). I plan to recruit 15–16 testers from the halfway house. Needs: a fresh signed AAB, privacy policy, data safety form, content rating, screenshots and listing text. The earlier AAB was full of bugs and is to be replaced.
- **Repo numbers:** 1 star, 0 forks, about 25 downloads in total. Nothing announced publicly yet (no social media for me).
