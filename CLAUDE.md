# CLAUDE.md

Repo conventions live in [AGENTS.md](AGENTS.md) — read it before touching anything here.

@AGENTS.md

## Skills

Six skills in `.claude/skills/` cover the recurring work in this repo. Invoke by name.

| Skill | Use when |
|---|---|
| `today` | "/today", "let's study", "what do I do today". Runs the day's timed DSA session from `dsa-patterns/PLAN.md` — 4 LeetCode problems, hints on the user's LeetCode code, 2h target / 1h minimum, then marks the day, updates `INDEX.md`, and logs the streak. |
| `study-module` | Adding a new topic/pattern module, upgrading a Compact/Partial module, adding worked problems or diagrams. Encodes the per-area file spec, README heading order, and index registration. |
| `revise` | "What should I revise?", "quiz me", logging a finished revision. Picks what is due, drills cheatsheet-first with active recall, writes the ladder state back into `INDEX.md`. |
| `review-attempt` | The user shares their own solution to an exercise. Reviews without handing over the answer; verifies by actually running the code. |
| `module-audit` | Checking module completeness or repo drift. Runs `.claude/skills/module-audit/scripts/audit.py`. |
| `pattern-triage` | An unseen problem — "which pattern is this?" Diagnoses via the recognition guides without solving it. |

## The two rules that matter most

1. **Never write a solution to anything in an `exercises.md`.** They are solution-free by
   design. Review, hint, and run their code instead — see `review-attempt`.
2. **Never commit.** Git here is local history only.
