# Registering a new module

An unregistered module is invisible to the revision system. Two or three files to touch.

## 1. Area `INDEX.md` — always

Add a row to the correct section table. New modules always start at `Not started` with empty
date/confidence cells — never pre-fill them.

**`dsa-patterns/INDEX.md`** (has a `Tier` column):
```
| [<Pattern>](<family>/<slug>/README.md) | Full | Not started | — | Study next | — |
```

**`js/INDEX.md`**, **`sys-design/INDEX.md`**, **`typescript-basics/INDEX.md`**
(`Format` column instead of `Tier` — `Full module` in js/ts, `Full repo` in sys-design):
```
| [<Topic>](<slug>/README.md) | Full module | Not started | — | Study next | — |
```

Also update, in the same file:
- The **"Recommended study order"** list for that section — insert the module at the point
  where its prerequisites are already covered, with the one-line reason the other entries
  carry ("generalizes to a variable-size contiguous range").
- Any **count** in the header prose ("all 33 patterns", "12 core-JavaScript modules"). These
  appear in `INDEX.md`, the area `README.md`, the root `README.md`, and `AGENTS.md`. Grep for
  the old number before assuming you found them all.

## 2. The area's lookup guide — always, if one exists

**`dsa-patterns/PATTERN-RECOGNITION-GUIDE.md`** — add a row under the matching *input shape*
section (Array/String, Linked list, Tree, Graph, 2D grid, …). The left cell is the signal in
the problem statement, phrased the way a problem statement would phrase it; the right cells
are the pattern link and its family:
```
| <signal, as a problem would word it> | [<Pattern>](<family>/<slug>/README.md) | <Family> |
```
If the new pattern is easily confused with an existing one, also add a line to the tie-break
table at the bottom of that file.

**`js/QUICK-RECALL-GUIDE.md`** — add rows under the right theme section, phrased as the
*interview question* someone would be asked:
```
| "<question as asked out loud>" | [<topic>](<slug>/README.md) |
```
One topic can legitimately own several question rows.

`sys-design/` and `typescript-basics/` have no separate lookup guide — their `INDEX.md` is it.

## 3. Cross-links from sibling modules — when it applies

If the new module is the "use this instead" answer for an existing module's *When NOT To Use*
or *Similar Patterns* section, add the link there too. This is how the repo stays navigable
from the inside rather than only from the index.

## Verify

```bash
python .claude/skills/module-audit/scripts/audit.py <area>/<path-to-module>
```
The audit fails if the module directory is not referenced from its area `INDEX.md`.
