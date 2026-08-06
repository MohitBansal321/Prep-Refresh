# Engineering Knowledge Repository — Index & Revision Tracker

This is the master index for the knowledge repo. Use it two ways:
1. **Find** a topic quickly.
2. **Revise** on a spaced-repetition schedule so topics move into long-term memory.

---

## How to revise (the flow)

For each topic, revise in tiers — do NOT re-read the full README:

1. **60 sec** — read `cheatsheet.md`.
2. **5 min** — skim the 3 diagrams in `images/`.
3. **Active recall** — answer the *Recall Questions* at the bottom of `cheatsheet.md` from memory, then rebuild the Easy/Medium exercise from `exercises.md` without looking at `code.ts`.
4. **Only if you miss something** — open the relevant README section. Nothing else.

## Spaced-repetition schedule

After each successful revision, set the next date using the ladder:

`Learned → +1 day → +3 days → +1 week → +2 weeks → +1 month → +3 months`

If you fail recall on a topic, drop it back one rung. Update the two date columns each time you revise.

---

## Structural Patterns

| Topic | Format | Status | Last Revised | Next Revision | Confidence (1-5) |
|-------|--------|--------|--------------|---------------|------------------|
| [Adapter](structural-patterns/adapter/README.md) | Full repo | Learned | 2026-08-01 | 2026-08-02 | 3 |
| [Composite](structural-patterns/composite/README.md) | Full repo | Not started | — | Study next | — |
| [Decorator](structural-patterns/decorator/README.md) | Full repo | Not started | — | Study next | — |
| [Facade](structural-patterns/facade/README.md) | Full repo | Not started | — | Study next | — |
| [Proxy](structural-patterns/proxy/README.md) | Full repo | Not started | — | Study next | — |

> **Legend — Status:** `Not started` = full repo ready, not yet studied · `Learned` = studied at least once, on the revision ladder. All five structural patterns now use the full template (README + code + exercises + cheatsheet + diagrams).

---

## Recommended study order (structural patterns)

These four pair up nicely — study them close together so the distinctions stick, since the #1 exam of understanding is telling them apart:

1. **Adapter** ✅ (done) — *changes* an interface.
2. **Facade** — *simplifies* many classes behind one entry point.
3. **Decorator** — *adds* behavior, keeps the same interface.
4. **Proxy** — *controls access*, keeps the same interface.
5. **Composite** — *tree* of part-whole objects treated uniformly.

When you finish studying one, change its Status to `Learned`, set Last Revised = today, and Next Revision = today + 1 day.

---

## Behavioral Patterns

| Topic | Format | Status | Last Revised | Next Revision | Confidence (1-5) |
|-------|--------|--------|--------------|---------------|------------------|
| [Strategy](behavioral-patterns/strategy/README.md) | Full repo | Not started | — | Study next | — |
| [Observer](behavioral-patterns/observer/README.md) | Full repo | Not started | — | Study next | — |
| [Command](behavioral-patterns/command/README.md) | Full repo | Not started | — | Study next | — |
| [State](behavioral-patterns/state/README.md) | Full repo | Not started | — | Study next | — |
| [Iterator](behavioral-patterns/iterator/README.md) | Full repo | Not started | — | Study next | — |

### Recommended study order (behavioral patterns)

1. **Strategy** — *swap* interchangeable algorithms (learn this first).
2. **State** — like Strategy structurally, but the object drives its own transitions (study right after Strategy to nail the distinction).
3. **Observer** — one change *fans out* to many listeners.
4. **Command** — a request *as an object* (undo/redo, queue, log).
5. **Iterator** — *traverse* a collection without exposing internals (ties into JS generators & async iterators).

---

## Creational Patterns

| Topic | Format | Status | Last Revised | Next Revision | Confidence (1-5) |
|-------|--------|--------|--------------|---------------|------------------|
| [Singleton](creational-patterns/singleton/README.md) | Full repo | Not started | — | Study next | — |
| [Factory Method](creational-patterns/factory/README.md) | Full repo | Not started | — | Study next | — |
| [Abstract Factory](creational-patterns/abstract-factory/README.md) | Full repo | Not started | — | Study next | — |
| [Builder](creational-patterns/builder/README.md) | Full repo | Not started | — | Study next | — |
| [Prototype](creational-patterns/prototype/README.md) | Full repo | Not started | — | Study next | — |

### Recommended study order (creational patterns)

1. **Factory Method** — the gateway creational pattern; learn the Simple-Factory-vs-Factory-Method-vs-Abstract-Factory distinction first.
2. **Abstract Factory** — families of related products (study right after Factory to nail the difference).
3. **Builder** — step-by-step construction of complex objects (telescoping-constructor fix).
4. **Prototype** — create by cloning; master shallow-vs-deep copy.
5. **Singleton** — one instance; learn *why it's often an anti-pattern* and prefer DI.

---

## Status overview — full GoF catalog complete

All 15 patterns across the three families are now full learning modules (README + code + exercises + cheatsheet + diagrams). The old single-file `.ts` notes have been removed.

| Family | Patterns | Format |
|--------|----------|--------|
| Creational | Singleton, Factory Method, Abstract Factory, Builder, Prototype | ✅ Full modules |
| Structural | Adapter, Composite, Decorator, Facade, Proxy | ✅ Full modules |
| Behavioral | Strategy, Observer, Command, State, Iterator | ✅ Full modules |

---

*Convention: each fully-studied topic lives in its own folder `topic-name/` with the template structure. Single-file `.ts` notes are the lightweight starting point before conversion.*
