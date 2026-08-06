---
name: design-patterns
description: Guide for learning, revising, and implementing GoF design patterns in TypeScript. Covers creational, structural, and behavioral pattern categories with concept explanations, code templates, real-world analogies, and practice exercises. Use this skill when asked about design patterns, SOLID principles, or system design patterns like Singleton, Factory, Builder, Prototype, Observer, Strategy, etc.
---

# Design Patterns (TypeScript)

Workflow for teaching/revising design patterns:

1. **Identify** which pattern the user needs (or help them choose)
2. **Load** the appropriate reference file
3. **Explain** the concept + when to use it (real-world analogy)
4. **Show** the code template with inline comments
5. **Present** a practice exercise for hands-on learning
6. **Review** the user's solution and give feedback

## Pattern Categories

| Category | Patterns | Reference |
|---|---|---|
| Creational | Singleton, Factory Method, Abstract Factory, Builder, Prototype | [creational-patterns.md](references/creational-patterns.md) |
| Structural | Adapter, Decorator, Facade, Proxy, Composite, Bridge, Flyweight | *(add `references/structural-patterns.md` when ready)* |
| Behavioral | Observer, Strategy, Command, State, Iterator, Mediator, Template Method | *(add `references/behavioral-patterns.md` when ready)* |

## Pattern Selection Guide

Help the user choose the right pattern:

- **"I need exactly one instance"** → Singleton
- **"I want to create objects without specifying exact class"** → Factory Method
- **"I need families of related objects"** → Abstract Factory
- **"My constructor has too many parameters"** → Builder
- **"I need to clone pre-configured objects"** → Prototype

## Teaching Template

For each pattern, follow this structure:
1. **One-line concept** — what it does
2. **When to use** — 2-3 bullet points
3. **The problem without it** — show the messy alternative
4. **Code template** — clean, typed, commented
5. **Practice exercise** — a realistic scenario with starter code and test cases
