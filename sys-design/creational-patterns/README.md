# Creational Patterns

## What are they?

Creational patterns deal with **object creation** — *how* instances get made.

Normally you create an object with `new SomeClass()`. That's fine until the *decision of
which class to instantiate* becomes complicated: maybe the type isn't known until runtime,
maybe construction takes many steps, maybe you must guarantee only one instance exists.
Creational patterns move that "how do I build this?" logic out of your business code.

## Why use them?

- **Decouple your code from concrete classes.** Your code depends on an interface; the
  pattern decides the actual class. Adding a new type doesn't touch existing logic.
- **Hide messy construction.** Complex setup (steps, config, dependencies) lives in one place.
- **Control instances.** Enforce rules like "exactly one" or "clone this instead of rebuilding."
- **Flexibility & extensibility.** Swap what gets created without rewriting the callers.

The common theme: **the client asks for an object, but doesn't hard-code how it's built.**

## The patterns in this folder

Each pattern is a full learning module — a folder with `README.md`, `code.ts`,
`exercises.md`, `cheatsheet.md`, and `images/` (class/flow/sequence Mermaid diagrams).
For quick revision, start at each pattern's `cheatsheet.md`. See the
[revision tracker](../INDEX.md) for the recommended study order and spaced-repetition schedule.

| Pattern | One-line purpose | Example used |
|---------|------------------|--------------|
| [Singleton](./singleton/README.md) | Guarantee a class has **only one instance**, globally accessible. | Config manager / DB connection pool (+ DI alternative) |
| [Factory Method](./factory/README.md) | Decide **which class to instantiate** without hard-coding `new`. | Notification senders (email/SMS/push) |
| [Abstract Factory](./abstract-factory/README.md) | Create **families of related objects** without naming concrete classes. | Cloud-provider resource families (AWS vs GCP) |
| [Builder](./builder/README.md) | Construct a **complex object step by step**; avoid giant constructors. | Fluent HTTP request builder |
| [Prototype](./prototype/README.md) | Create new objects by **cloning** an existing one. | Cloning campaign templates from a registry |

## How to pick one

- Need **only one** of something (logger, config, DB pool)? → **Singleton**
- Deciding **one type** at runtime, want subclasses to extend? → **Factory Method**
- Deciding a **whole family** of matching types together? → **Abstract Factory**
- Object has **many optional parts / multi-step assembly**? → **Builder**
- Cheaper to **copy an existing object** than build fresh? → **Prototype**
