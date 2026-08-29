# Module spec, per area

Exact file sets and heading orders. Deviating breaks the drill and the audit.

---

## `dsa-patterns/<family>/<pattern>/` — Full tier

```
README.md
code.cpp
exercises.md
cheatsheet.md
images/recognition-diagram.md
images/flow-diagram.md
images/trace-diagram.md
problems/01-<leetcode-slug>.cpp
problems/02-<leetcode-slug>.cpp
problems/03-<leetcode-slug>.cpp
problems/04-<leetcode-slug>.cpp
problems/README.md
```

**README heading order** (`##` level, reference: `tree-graph-patterns/tree-bfs/README.md`):

`Intent` · `Real Life Analogy` · `Problem` · `Solution` · `Architecture` ·
`Why Not Other Approaches?` · `Diagrams` · `The Code` · `Tradeoffs` · `Complexity` ·
`Common Mistakes` · `When To Use` · `When NOT To Use` · `Where This Shows Up` ·
`Similar Patterns` · `Interview Discussion` · `Key Takeaways` · `Further Reading`

`Why Not Other Approaches?` sits *after* Solution/Architecture, not before: a reader should
meet the pattern's own mechanism before being asked to judge alternatives to it.

Open with a short "Pick your depth" block (`###`) routing the reader to cheatsheet vs README
vs problems depending on how much time they have.

**`code.cpp` contract** — C++17, standalone, no external headers beyond the STL. `main()`
runs named cases and prints `[PASS] <case description> -> <expected result>`. Assert as well
as print, so a regression fails loudly under `&&`. Group output by function with
`--- functionName ---` separators.

**`problems/` contract** — 4 real LeetCode problems, each a standalone `.cpp` with its own
`main()`, a header comment naming the problem/number/difficulty, and heavy inline commentary
explaining *why*, not what. Pick them so each covers a different facet of the pattern
(e.g. for Tree BFS: collect a level, reorder a level, early-exit, mutate the tree). The
`problems/README.md` carries a table (Name · LeetCode # + link · Difficulty · one-line
approach · Time/Space · file link) and a **"Why these four"** section explaining the facet
each one owns and how it differs from the previous.

**`images/` contract**
- `recognition-diagram.md` — a `flowchart TD` that takes an unseen problem statement and
  routes it to this pattern or to the pattern it is most often confused with. Decision nodes
  are the *signals*, not the algorithm.
- `flow-diagram.md` — the algorithm's control flow.
- `trace-diagram.md` — one concrete input traced step by step, with the data structure's
  contents shown at each step.

Each file: `# <Pattern> — <Kind> Diagram`, one paragraph on when to use this diagram, one
```mermaid fence, then a `## How to read it` section.

---

## `sys-design/<category>-patterns/<pattern>/`

```
README.md
code.ts
exercises.md
cheatsheet.md
images/class-diagram.md
images/sequence-diagram.md
images/flow-diagram.md
```

**README heading order** (reference: `structural-patterns/facade/README.md`, and the source
template in `repository_template_prompt.md`):

`Intent` · `Real Life Analogy` · `Problem` · `Why Not Other Solutions?` · `Solution` ·
`Architecture` · `Execution Flow` · `Class Diagram` · `Sequence Diagram` · `Flow Diagram` ·
`Implementation` · `Code Walkthrough` · `Advantages` · `Disadvantages` · `Tradeoffs` ·
`Complexity` · `Performance Considerations` · `Common Mistakes` · `When To Use` ·
`When NOT To Use` · `Real Production Examples` · `Where I Can Use This` · `Similar Patterns` ·
`Interview Discussion` · `Summary` · `Key Takeaways` · `Further Reading`

The three diagrams are embedded inline in the README *and* live in `images/` — keep them
identical.

**`code.ts` contract** — strict-mode TypeScript, runs under `npx ts-node`. Dependency
injection where the pattern implies it, SOLID, no toy `class Animal` examples: use a backend
scenario the audience would actually ship (payments, media pipeline, tenant config, RAG).
`structural-patterns/` is the only directory with a `package.json` (`@types/node`).

---

## `js/<topic>/`

```
README.md
code.js
exercises.md
cheatsheet.md
```

No `images/`. README structure is topic-driven rather than fixed — a multi-part topic splits
into `## Part A — …` / `## Part B — …` / `## How the two halves connect` (see
`js/this-and-prototypes/README.md`) — but it **always** ends with
`## Summary` · `## Key Takeaways` · `## Further Reading`.

`code.js` runs with `node js/<topic>/code.js` and prints labelled sections with expected
output in comments.

---

## `typescript-basics/<topic>/`

```
README.md
code.ts
cheatsheet.md
```

Same heading order as `sys-design`, except `Similar Patterns` becomes
`Related TypeScript Features`. `exercises.md` is optional here and most topics omit it —
add one only if asked.

---

## Cheatsheet spine (identical in all four areas)

```markdown
# <Topic> — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | … |
| **Recognition Signal** / **Intent** | … |
| **Problem** | … |
| **Solution** | … |
| **Participants** (patterns) / **Time / Space Complexity** (DSA) | … |
| **Flow** | … |
| **Pros** | … |
| **Cons** | … |
| **Use When** | … |
| **Avoid When** | … |
| **Real Examples** | … |
| **Related Topics** | … |

### <Optional contrast block — e.g. "Thin vs Leaky Facade">

### Skeleton
<the minimal code shape, compilable, ~15-40 lines>

### Remember In One Sentence
> **<one sentence that carries the actual mechanism — see writing-standards.md>**

### Two Facts People Get Wrong
- <misconception as a question>? **No** — <what actually happens, incl. the silent failure mode>.
- <second one>

---

## Recall Questions (answer from memory — no peeking)

<one line on how to use them, then 8-10 numbered questions>
```

The table cells are dense — full sentences separated by `·`, not one-word fragments. A
cheatsheet row that says "Pros: fast, simple" is useless at revision time.
