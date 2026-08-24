# Drill protocols and ladder mechanics

## Per-area drill

The "rebuild from memory" step differs by area. Pick the right one — rebuilding the wrong
artifact tests the wrong thing.

### `dsa-patterns/`
1. `cheatsheet.md` (60s) → skim `images/recognition-diagram.md` and `images/trace-diagram.md`.
2. Recall Questions, one at a time.
3. **Rebuild:** re-derive **one `problems/*.cpp` solution from scratch**, in a scratch file —
   not `code.cpp`, which they may half-remember verbatim. Rotate which problem across
   sessions. Compile it:
   ```bash
   g++ -std=c++17 -Wall /tmp/attempt.cpp -o /tmp/out && /tmp/out
   ```
4. **Bonus check that catches fake recall:** hand them a problem statement from
   `PATTERN-RECOGNITION-GUIDE.md` belonging to a *neighbouring* pattern and ask whether this
   pattern applies. Knowing when it does *not* apply is the actual interview skill.

### `sys-design/`
1. `cheatsheet.md` (60s) → skim `images/class-diagram.md` and `images/sequence-diagram.md`.
2. Recall Questions.
3. **Rebuild:** name the participants and their responsibilities from memory first, *then*
   write the Easy/Medium exercise from `exercises.md`. Run with `npx ts-node`.
4. **Bonus:** ask for the nearest-neighbour pattern and the one property that separates them
   (Adapter vs Facade, Proxy vs Decorator, Strategy vs State). Confusing these is the most
   common real failure.

### `js/`
1. `cheatsheet.md` (60s).
2. Recall Questions.
3. **Rebuild:** the Easy/Medium exercise from `exercises.md`, run with `node`.
4. **Bonus:** a "what does this print?" snippet built from the module's Common Mistakes
   section. Ask for the output *and* the reason before running it.

### `typescript-basics/`
1. `cheatsheet.md` (60s).
2. Recall Questions.
3. **Rebuild:** reconstruct the `code.ts` skeleton — signatures and type parameters, not
   bodies. Typecheck it: `npx ts-node <file>`.
4. **Bonus:** ask what error the compiler produces if a constraint is removed, then remove it
   and check.

---

## Ladder

Rungs, in order:

| Rung | Interval | 
|---|---|
| 0 | +1 day |
| 1 | +3 days |
| 2 | +1 week (7d) |
| 3 | +2 weeks (14d) |
| 4 | +1 month (30d) |
| 5 | +3 months (90d) |

**Infer the current rung** from the existing row: `interval = Next Revision − Last Revised`,
matched to the table above. If `Status` is `Not started` or the dates are `—`, the card is
pre-rung 0.

**Transitions**
- Pass on a `Not started` card → `Status: Learned`, rung 0, `Next Revision = today + 1 day`.
- Pass → rung + 1 (cap at rung 5; a card at 5 that passes stays at 5, +3 months again).
- Fail → rung − 1 (floor at rung 0, i.e. +1 day). Status stays `Learned`.
- `Last Revised` is always today. `Confidence` is always overwritten with this session's score.

**Compute the date with the shell, never by hand:**
```bash
date +%F -d "+3 days"      # GNU date, available via the Bash tool on this machine
```

## Editing the row

Edit exactly one line. Preserve the leading module link cell and the `Tier`/`Format` cell
verbatim — only the last four cells change.

Before (`dsa-patterns/INDEX.md`):
```
| [Tree BFS](tree-graph-patterns/tree-bfs/README.md) | Full | Not started | — | Study next | — |
```
After a first pass on 2026-08-24 with confidence 4:
```
| [Tree BFS](tree-graph-patterns/tree-bfs/README.md) | Full | Learned | 2026-08-24 | 2026-08-25 | 4 |
```

After a later *failed* recall on a card sitting at +2 weeks (rung 3 → rung 2, so +1 week):
```
| [Tree BFS](tree-graph-patterns/tree-bfs/README.md) | Full | Learned | 2026-09-20 | 2026-09-27 | 2 |
```

Column layouts differ slightly between the four INDEX files (`Tier` vs `Format`) — read the
header row of the file you are editing rather than assuming. Never reformat or re-sort the
table, and never touch rows other than the one drilled.

## Session hygiene

- Two to four cards is a session. More than that and recall quality collapses — say so and
  stop rather than grinding through the queue.
- If the same card fails twice in a row, don't just demote it: the module itself may be the
  problem. Point at the specific README section and suggest re-reading it properly once,
  outside a drill.
- Never mark a card passed that the user did not actually attempt.
