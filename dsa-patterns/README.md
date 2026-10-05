# DSA Patterns

> **▶ Start here: [`PLAN.md`](./PLAN.md)** — 36 patterns in 6 weeks, one a day, 4 LeetCode problems each. Type `/today` in Claude and it
> tells you exactly what to do next. You don't need to read anything below this line to begin.

Almost every array/string/tree/graph interview question is a disguised version of one of a small number of **patterns**. Learn the pattern — what signal in the problem statement triggers it, what the template looks like, what its complexity is — and you can solve any problem that fits it, not just the one you've memorized. That's the philosophy of this repo: **study patterns, not individual problems.**

All code is **C++ (C++17)**, chosen because it forces you to think explicitly about pointers, memory, and complexity — exactly the muscles these patterns exercise.

## The 8 families

| Family | What ties its patterns together | The core question it answers |
|--------|-----------------------------------|-------------------------------|
| [Array & String Patterns](array-string-patterns/) | Replace an O(n²) brute-force scan with one linear pass over running state | "Can I avoid re-scanning the array on every step?" |
| [Linked List Patterns](linked-list-patterns/) | Work around "no random access, O(1) space only" using pure pointer arithmetic | "Can I solve this by moving pointers instead of copying data?" |
| [Searching & Sorting Patterns](searching-sorting-patterns/) | Exploit sorted input or a heap's ordering guarantee | "Can I halve the search space, or maintain a small ordered structure, instead of re-sorting?" |
| [Tree & Graph Patterns](tree-graph-patterns/) | Traverse (BFS/DFS) or track connectivity without traversing at all | "Do I need level order, path order, dependency order, or just connectivity?" |
| [Recursion & Backtracking Patterns](recursion-backtracking-patterns/) | Explore a decision tree, either enumerating everything or pruning invalid branches early | "Do I need every possibility, or can I cut a branch as soon as it's invalid?" |
| [Dynamic Programming Patterns](dynamic-programming-patterns/) | Define state, its dependencies, and a fill order to avoid recomputing overlapping subproblems | "What does `dp[i]` (or `dp[i][j]`) mean, and what does it depend on?" |
| [Greedy Patterns](greedy-patterns/) | Sort by the right key, commit to one locally-optimal choice per step, never revisit it | "Can I prove the local choice is always safe?" |
| [Advanced Data Structure Patterns](advanced-ds-patterns/) | Build a purpose-specific structure that turns a slow repeated operation fast | "Is there a structure that answers this query in less than O(n) after some upfront cost?" |

## How to read this repo

Each family folder has its own `README.md` explaining the family and how to tell its patterns apart. Every pattern is a **Full module** — a folder per pattern with `README.md`, `code.cpp`, `exercises.md`, `cheatsheet.md`, worked `problems/` (4 fully-solved C++ problems), and `images/` (Mermaid diagrams).

All 36 patterns are built to this format and every `.cpp` file compiles and passes its tests — see [`INDEX.md`](./INDEX.md) for the tracker.

For studying and revision, use the [revision tracker (INDEX.md)](./INDEX.md) — it lists every pattern's build status, the recommended study order, and a spaced-repetition schedule.

### Anatomy of a full pattern module

```
pattern-name/
├── README.md              # the deep-dive: intent, recognition signal, template, complexity, mistakes, interview discussion
├── code.cpp                # generic, heavily-commented, reusable template for the pattern
├── exercises.md             # Easy / Medium / Hard / Real-world / Bonus practice problems — no solutions
├── cheatsheet.md            # one-minute revision sheet + recall questions
├── problems/
│   ├── README.md            # index of the 4 worked problems below (name, difficulty, approach, complexity)
│   └── 0N-problem-name.cpp  # fully worked, commented C++ solution
└── images/
    ├── recognition-diagram.md  # Mermaid flowchart: problem signals → "use this pattern"
    ├── flow-diagram.md         # Mermaid flowchart of the algorithm's control flow
    └── trace-diagram.md        # Mermaid diagram tracing pointer/window state on a concrete example
```

`exercises.md` and `problems/` serve different purposes: `problems/` teaches the pattern through **worked examples with solutions**; `exercises.md` is for **practicing it yourself**, deliberately without solutions.

### Compiling the code

Every `.cpp` file is standalone. Compile and run any of them with:

```bash
g++ -std=c++17 -Wall path/to/file.cpp -o /tmp/out && /tmp/out
```

## Start here

- **New to DSA, want comprehensive mastery, or refreshing after a long break?** → [`LEARNING-PATHS.md`](./LEARNING-PATHS.md) has a specific path for each of those, plus a path for "I only need one family right now." Read that first — it tells you how to move through everything below.
- Comfortable with arrays/strings, want the highest-leverage patterns first → start with [Two Pointers](array-string-patterns/two-pointers/README.md), then [Sliding Window](array-string-patterns/sliding-window/README.md).
- Ready to extend pointer techniques to linked lists → [Fast & Slow Pointers](linked-list-patterns/fast-slow-pointers/README.md).
- **Staring at a new problem and not sure which pattern it needs?** → [`PATTERN-RECOGNITION-GUIDE.md`](./PATTERN-RECOGNITION-GUIDE.md) is a single lookup table across all 36 patterns, organized by input shape and problem signal — the fastest way in. Each pattern's own **Recognition Signal** section (or Recognition Diagram, for Full-tier modules) goes deeper once you've narrowed it down.
