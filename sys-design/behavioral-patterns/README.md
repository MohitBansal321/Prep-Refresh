# Behavioral Patterns

## What are they?

Behavioral patterns deal with **communication and responsibility between objects** — the
*algorithms* and *flow of control* at runtime, not how objects are built or wired together.

They answer questions like: "Who does what?", "How do these objects talk without depending
on each other's guts?", and "How do I change behavior at runtime instead of with big
`if/else` chains?"

## Why use them?

- **Reduce coupling** between the objects that interact.
- **Swap behavior at runtime** instead of hard-coding it with conditionals.
- **Isolate what varies** — put each behavior/algorithm in its own class.
- **Support undo, queuing, notifications, and state machines** cleanly.
- **Kill giant `switch`/`if-else` blocks** by turning each branch into an object.

The common theme: **assign responsibilities and let objects collaborate flexibly.**

## The patterns in this folder

Each pattern is a full learning module — a folder with `README.md`, `code.ts`,
`exercises.md`, `cheatsheet.md`, and `images/` (class/flow/sequence Mermaid diagrams).
For quick revision, start at each pattern's `cheatsheet.md`. See the
[revision tracker](../INDEX.md) for the recommended study order and spaced-repetition schedule.

| Pattern | One-line purpose | Example used |
|---------|------------------|--------------|
| [Strategy](./strategy/README.md) | Swap **interchangeable algorithms** at runtime. | Shipping-cost calculation strategies |
| [Observer](./observer/README.md) | **Notify many objects** automatically when one changes. | OrderService fan-out (email/inventory/analytics/audit) |
| [Command](./command/README.md) | Turn a **request into an object** (queue, log, undo). | Operations processor with undo/redo history |
| [State](./state/README.md) | Change an object's **behavior when its state changes**. | Order lifecycle state machine |
| [Iterator](./iterator/README.md) | Traverse a collection **without exposing its internals**. | Collections + paginated async API cursor |

## Strategy vs State (they look alike — how to tell apart)

- **Strategy**: *you* pick the algorithm from outside; the strategies don't know about each other.
- **State**: the object switches its *own* behavior internally, and states can trigger
  transitions to other states.

## How to pick one

- Multiple ways to do one job, chosen at runtime? → **Strategy**
- One change must fan out to many listeners? → **Observer**
- Need undo/redo, queuing, or logging of actions? → **Command**
- Behavior depends on a lifecycle/mode with transitions? → **State**
- Need to loop over a collection uniformly? → **Iterator**
