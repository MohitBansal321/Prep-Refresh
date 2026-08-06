# Structural Patterns

## What are they?

Structural patterns deal with **how objects and classes are composed** into larger structures.
Once objects exist, how do you *assemble* them so the result is flexible and easy to change?

These patterns are mostly about **relationships** — wrapping one object in another, plugging
incompatible things together, or presenting many objects as a single simplified unit.

## Why use them?

- **Make incompatible things work together** without rewriting either side.
- **Add behavior/responsibilities** to an object without editing its class or exploding
  the number of subclasses.
- **Simplify a complex subsystem** behind one clean entry point.
- **Control or defer access** to expensive or sensitive objects.
- **Treat individual objects and groups of objects the same way.**

The common theme: **compose objects to gain flexibility, without changing the objects themselves.**

## The patterns in this folder

Each pattern is a full learning module — a folder with `README.md`, `code.ts`,
`exercises.md`, `cheatsheet.md`, and `images/` (class/flow/sequence Mermaid diagrams).
For quick revision, start at each pattern's `cheatsheet.md`. See the
[revision tracker](../INDEX.md) for the recommended study order and spaced-repetition schedule.

| Pattern | One-line purpose | Example used |
|---------|------------------|--------------|
| [Adapter](./adapter/README.md) | **Translate** one interface into another the client expects. | Multi-provider payment gateway (Stripe/Razorpay) |
| [Decorator](./decorator/README.md) | **Add responsibilities dynamically** by wrapping an object. | HTTP client + Logging/Retry/Cache/RateLimit |
| [Facade](./facade/README.md) | Expose a **simple interface** over a complex subsystem. | OrderCheckout over Inventory/Payment/Shipping |
| [Composite](./composite/README.md) | Treat **individual objects and trees of objects uniformly**. | Cloud file-browser tree (files + directories) |
| [Proxy](./proxy/README.md) | A **stand-in** that controls access to a real object. | Caching/protection proxy over a slow service |

## Adapter vs Decorator vs Proxy (they all "wrap" — how to tell apart)

- **Adapter** changes the *interface* (makes A look like B). Purpose: compatibility.
- **Decorator** keeps the *same interface* but adds *behavior*. Purpose: extend features.
- **Proxy** keeps the *same interface* but controls *access* (lazy load, security, logging).
  Purpose: gatekeeping.

## How to pick one

- Two interfaces don't match? → **Adapter**
- Want to stack optional features at runtime? → **Decorator**
- Want to hide a tangled subsystem behind one door? → **Facade**
- Working with tree/nested hierarchies? → **Composite**
- Need to guard, delay, or monitor access to an object? → **Proxy**
