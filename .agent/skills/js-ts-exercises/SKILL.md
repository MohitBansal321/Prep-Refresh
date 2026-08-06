---
name: js-ts-exercises
description: Generate, review, and grade practice exercises for JavaScript and TypeScript concepts including design patterns. Use this skill when the user wants to practice coding, solve challenges, get exercise ideas, or have their solutions reviewed. Triggered by requests like "give me an exercise", "quiz me", "review my code", "practice problems", or "coding challenge".
---

# JS/TS Exercise Generator & Reviewer

## Workflow

Determine the user's intent:

**"Give me an exercise"** → Generation workflow (below)
**"Review my code"** → Review workflow (below)
**"Quiz me"** → Quick quiz workflow (below)

## Generation Workflow

1. Ask topic if not specified (or pick from exercise catalog)
2. Ask difficulty: **Easy** (concept demo) / **Medium** (apply to scenario) / **Hard** (combine patterns, edge cases)
3. Present the exercise with:
   - Scenario description
   - Requirements as numbered list
   - Starter code skeleton
   - Test code (commented out, to be uncommented after solving)
4. Wait for solution, then review

## Review Workflow

1. Read the user's submitted code
2. Check against the exercise requirements
3. Evaluate:
   - ✅ Correctness — does it work?
   - ✅ Pattern adherence — does it follow the pattern correctly?
   - ✅ TypeScript types — are types used properly?
   - ✅ Edge cases — are they handled?
4. Give feedback with specific line references
5. If issues found, give a hint (not the answer) and let them retry

## Quick Quiz Workflow

1. Pick a concept from the exercise catalog
2. Show a short code snippet and ask "what does this output?"
3. Reveal answer with explanation after user responds

## Exercise Catalog

See [exercise-catalog.md](references/exercise-catalog.md) for ready-made exercises organized by topic and difficulty.

## File Conventions

When creating exercise files in this repo:
- JS exercises go in `js/` directory
- Design pattern exercises go in `sys design/<category>/`
- Use descriptive filenames: `<topic>_exercise.ts`
- Include test code as commented block at the bottom
