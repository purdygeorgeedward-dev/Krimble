# AGENTS.md — read this first, every session

Krimble: a mobile-first Android fork of Krita, aiming at industry-standard editor parity. Owner: George Edward Purdy. He works from his phone.

## Where we are (update this section after every build or release)
- Working branch: `fork-1.0.71` (1.0.71 plus picked changes, nothing from 1.0.72).
- Last build: 82 (`1.0.82-test-fork71`, Nov 27 2026). Next build number: 83.
- Last public release: v1.0.80-beta. History: `docs/releases/`.
- Open bugs: the newest list in `KRIMBLE_BUG_LISTS.md`. Older lists are history, not work.
- What changed since the last build: newest entries at the end of `KRIMBLE_CHANGES.md`.

**Never work on a bug from an older build unless George raises it now.** Check the build it came from first.

## Rules for talking to George
- ABC: accuracy, brevity, clarity. No text walls. No bare 4-word replies either.
- "Can you…?" is a question. Answer yes or no, then stop.
- Before any change: say in one line what you understood. Wait for his yes.
- Visual topics: show a picture (rendered from the real code), not words.
- Don't guess what his screenshots mean. Ask.
- His image names and notes are always kept (the chat app renames uploads; his name is in the message note). Renaming files for the work is fine, but your guesses about what an image means never go into a name or a note.
  Why: agents keep assuming wrong things about what an image is for, then leave notes that pass the error on to later sessions. Ask questions, then update the notes with his answers.
- Don't record your own guesses as his bugs or decisions.
- One topic at a time. Wait for his answer.
- If he's about to make a big mistake, warn him (General Order 1).

## Rules for the repo
- Push only when he says push. Every push carries `[skip ci]`.
- Build only when he says BUILD. One build at a time.
- Before a build: list what's in it and what's still open. Wait for his okay.
- Nothing from 1.0.72 goes into `fork-1.0.71`, not even a derivative.
- Log every change in `KRIMBLE_CHANGES.md` with date, time (MST) and his words.
- Save every screenshot he sends in `docs/krimble-refs/`.
- Components do what they do. Don't invent rules nobody asked for.
- Never restore a Krita default over a Krimble change.
- Never write "Photoshop" in public text. Say "industry standard".

## Design rules
See `KRIMBLE_DESIGN_RULES.md`. Color Lab is protected: change it only when he names it.
