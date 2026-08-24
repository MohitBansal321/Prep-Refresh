---
name: module-audit
description: Check study modules in this repo against the template invariants — required files, README heading order, cheatsheet recall questions, solution leakage in exercises, broken relative links, INDEX registration, mermaid diagrams, and C++ compilation. Use before finishing a module, when checking whether the repo has drifted, when a link or file seems missing, or when asked to verify or fix a module's completeness.
allowed-tools: Read, Grep, Glob, Edit, Bash
---

# Module audit

67 modules built to one template will drift — a `problems/` file referenced but never written,
a `trace-diagram.md` with prose and no mermaid fence, a module missing from `INDEX.md` so it is
never revised. The script finds all of it mechanically. Your job is to interpret and fix, not
to eyeball files.

## Run it

```bash
python .claude/skills/module-audit/scripts/audit.py                      # whole repo
python .claude/skills/module-audit/scripts/audit.py dsa-patterns         # one area
python .claude/skills/module-audit/scripts/audit.py js/event-loop        # one module
python .claude/skills/module-audit/scripts/audit.py --quiet              # failures only
python .claude/skills/module-audit/scripts/audit.py <path> --compile     # + g++ every .cpp
```

`--compile` is slow across the whole repo (165 files) — scope it to an area or module unless
the user asked for a full sweep. Exit code is 1 when any FAIL is present.

## FAIL vs WARN

**FAIL** — the module is broken as a study artifact:
- missing a required file for its area, or missing `problems/README.md`
- **no `## Recall Questions` in the cheatsheet** — the module cannot enter the ladder, so it
  will never be revised
- a broken relative link (usually a `problems/*.cpp` that was promised in the README and never
  written)
- a diagram file with no ```mermaid fence
- not registered in the area `INDEX.md` — invisible to the revision system
- a `.cpp` that doesn't compile under `--compile`

**WARN** — worth fixing, not urgent: a missing README heading, fewer than 8 recall questions,
`problems/` with fewer than 4 solutions, `-Wall` warnings, and the leaked-solution heuristic.

## Fixing

**Report first, then propose.** Group the findings by root cause, not by file — "three modules
reference `problems/` files that were never written" is actionable; forty link errors are not.
Then ask before doing anything that writes study content.

Safe to fix directly (mechanical, no content judgement):
- correcting a link path where the target clearly exists under a different name
- adding the missing `INDEX.md` row (Status `Not started`, empty dates)
- deleting a dead link to a file that was never meant to exist

Never fix silently — these need the user, because the fix *is* study material:
- writing a missing `problems/*.cpp` or `problems/README.md` → that's [study-module](../study-module/SKILL.md)
- writing missing recall questions or README sections → same
- **a "possible leaked solution" warning.** Read the block first. A skeleton, a signature, or a
  starter struct is fine and should stay. A working implementation must be removed — but the
  exercise then needs rewording, which is the user's call, not a silent deletion.

## Known heuristics that can be wrong

- **Leaked solutions** flag any 12+ line fenced block with three or more control-flow keywords.
  Setup code (a `TreeNode` definition, a NestJS service stub) trips it legitimately. Always read
  the block before acting.
- **README headings** are checked as a set, not a sequence, and `js/` modules are intentionally
  freeform apart from the closing three sections. A missing heading in `js/` is often correct.
- The script does not check prose quality. A module can pass every check and still be badly
  written — for that, see
  [study-module writing standards](../study-module/references/writing-standards.md).
