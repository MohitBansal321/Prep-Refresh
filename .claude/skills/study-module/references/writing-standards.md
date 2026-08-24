# Writing standards

The audience (from `repository_template_prompt.md`) is a backend engineer with ~2 years on
Node/TypeScript/NestJS/Postgres/Redis/Docker who wants to be a Staff engineer in five years.
Not a student, not an interview crammer. Write like a mentor who has debugged this in
production, not like documentation.

## The five rules that actually separate this repo's prose from generated filler

**1. Name the mechanism, then explain it.** Vague: "BFS processes nodes level by level."
Mechanism: "read `level_size = q.size()` into a named local *before* the inner loop — that
one line freezes the current level's membership, so children pushed during the loop become
the next level instead of merging into this one." The mechanism is the one sentence the
reader must retain; everything else supports it.

**2. Anchor every claim to a real line.** Reference actual files and identifiers:
"`minDepth` and `levelOrder` in [code.cpp](code.cpp) share an identical skeleton — name every
line that differs." If a paragraph could have been written without opening the code file,
delete it.

**3. Prefer the smallest breaking input to an adjective.** Don't say an approach is "fragile."
Construct the input that breaks it: "a root whose right child is a leaf and whose left child
has a left child of its own." Wrong answers get spelled out, not gestured at.

**4. Name the *silent* failure.** The valuable half of a common mistake is that it doesn't
crash. "Writing `for (i = 0; i < q.size(); ++i)` still visits every node in FIFO order and
never crashes; it just fuses adjacent levels — a silent correctness bug rather than a visible
failure." Always answer: what does the reader *see* when they get this wrong?

**5. Why before how, always.** `Problem` → `Why Not Other Solutions?` → `Solution` →
`Architecture` come before any code. The `Solution` section explains the thinking; it does not
show the implementation.

## Section-specific requirements

- **Real Life Analogy** — one concrete everyday scene, then immediately say where the analogy
  *breaks down*. An analogy with no stated limit teaches a wrong model.
- **Why Not Other Solutions?** — name the two or three approaches a competent engineer would
  actually reach for first, and the specific property each one lacks. Not strawmen.
- **Common Mistakes** — for each: what it looks like, why it happens (the mental model behind
  it), what the symptom is, how to avoid it.
- **Interview Discussion** — what *experienced engineers* discuss, the standard follow-ups,
  and the misconceptions. Not a question list.
- **Real Production Examples** — real, checkable systems: NestJS/Spring service layers,
  `fs.promises`, Prisma/TypeORM, AWS SDK clients, Postgres internals. Never "many
  applications use this."
- **Remember In One Sentence** — one long sentence that survives on its own six months later,
  containing the mechanism, the cost, and what you buy with it. If it reads like a definition,
  rewrite it.
- **Recall Questions** — 8-10, answerable *only* by someone who understood this module, each
  citing a specific file or function. Good: "why is `prev` declared inside the outer loop —
  describe exactly what the tree looks like afterward if it is hoisted, and why nothing
  crashes." Bad: "what is BFS?"

## Exercise standards

Every exercise: scenario → **Task** (what to build, and which file to start from) →
**Think about** (the trap to reason through *before* typing) → **Then answer** (a question
that forces the reader into another file in the module).

- Easy = adapt a worked file with one substitution. Medium = same skeleton, different
  commitment point. Hard = a genuinely different construction. Real-World = a backend
  scenario in NestJS/Postgres/Redis terms with the same shape. Bonus = the classic follow-up.
- The Real-World Challenge must be a system the reader could plausibly own — tenant hierarchy
  rollout, cache invalidation fan-out, job queue draining — not "imagine a social network."
- **No solutions.** Starter signatures and skeletons are fine. Solutions are not.

## Banned constructions

- "In this section we will explore…", "It is important to note that…", "Let's dive in."
- Adjective stacks with no content: "powerful, flexible, and elegant."
- Bullet lists of one-word items where a sentence carries the reasoning.
- Claims about performance with no mechanism ("this is faster") — say *what* work is avoided.
- Emoji in module prose. (Status legends in `INDEX.md` are the only exception, and they
  already exist — match them, don't add more.)
