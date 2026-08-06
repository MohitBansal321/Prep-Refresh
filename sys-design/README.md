# Design Patterns

Design patterns are **proven, reusable solutions to common problems** in software design.
They aren't code you copy-paste — they're *blueprints* you adapt to your situation.

The classic "Gang of Four" catalog splits 23 patterns into **three families**, based on
*what problem they solve*:

| Family | What it's about | The core question it answers |
|--------|-----------------|------------------------------|
| [Creational](./creational-patterns/) | **How objects get created** | "How do I build this object without hard-coding its class?" |
| [Structural](./structural-patterns/) | **How objects are composed** | "How do I combine objects into bigger structures cleanly?" |
| [Behavioral](./behavioral-patterns/) | **How objects talk & share responsibility** | "How do objects communicate without being tightly coupled?" |

## How to read this repo

Each family folder has its own `README.md` explaining the family. Individual patterns come
in **two formats** (the repo is mid-migration from the first to the second):

- **Full module** (the target format) — a folder per pattern with `README.md`, `code.ts`,
  `exercises.md`, `cheatsheet.md`, and `images/` diagrams. All **structural** patterns use this.
- **Single-file note** (the older, lighter format) — one `.ts` file with a comment block:
  **Concept**, **Why use it?**, and a runnable **Example**. **Creational** and **behavioral**
  patterns are still in this format, to be converted as they're studied deeply.

For studying and revision, use the [revision tracker (INDEX.md)](./INDEX.md) — it lists every
pattern's status, the recommended study order, and a spaced-repetition schedule.

Start here → then open the folder that matches your problem:

- Building/instantiating things is awkward → **creational**
- Fitting pieces together / wrapping things → **structural**
- Coordinating logic and communication at runtime → **behavioral**
