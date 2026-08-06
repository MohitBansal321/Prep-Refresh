# Composite Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting part-whole hierarchies and pushing recursion *into the structure* instead of the client.

> Rule of thumb for every exercise: the **Client must contain zero `instanceof` checks and zero traversal loops**. Leaves answer about themselves and stop the recursion; composites delegate to children and aggregate. Decide consciously whether child-management methods live on the shared interface (transparency) or only on the composite (safety).

---

## Easy — Menu Tree

You are building a restaurant menu. A menu has sections (Drinks, Mains, Desserts), and a section can contain individual items **or** sub-sections (Mains → Pasta, Mains → Grill).

Define a `MenuComponent` interface:

```ts
interface MenuComponent {
  getName(): string;
  getPrice(): number;      // an item returns its price; a section returns the sum
  print(indent?: string): void;
}
```

**Task:** Implement `MenuItem` (leaf, has a fixed price) and `MenuSection` (composite, holds children and can nest). `getPrice()` on a section must recursively sum all items beneath it. Build a menu with at least one section nested inside another and print it.

**Acceptance:** Calling `getPrice()` on the top-level menu returns the total price of every item in the entire tree, and `print()` shows the hierarchy with indentation.

---

## Medium — Nested Category Tree with Counts (Safety Variant)

Model an e-commerce category tree: `Category` nodes can contain sub-categories, and `Product` nodes are leaves.

**Task:**
- Define a `CatalogNode` interface with `getName()`, `countProducts()`, and `render(indent?)`.
- Put child-management (`add`, `remove`) **only** on `Category` (the *safety* variant) — a `Product` must not even expose `add()`.
- `countProducts()` on a category recursively counts all products in its subtree; on a product it returns `1`.
- Add a `findCategory(name): Category | null` that recursively searches the tree.

**Bonus constraint:** `getChildren()` on a `Category` must return a **read-only** view so callers cannot mutate the internal array and bypass your bookkeeping.

---

## Hard — Boolean Rule Engine

Build a rule engine used for feature-flag targeting or fraud detection. A rule is a tree of boolean logic evaluated against a context object.

Node types:
- `ConditionLeaf` — a leaf that tests one fact, e.g. `country === "IN"` or `amount > 1000`.
- `AndNode`, `OrNode` — composites that combine their children.
- `NotNode` — a composite with exactly one child that negates it.

Common interface:

```ts
interface Rule {
  evaluate(context: Record<string, unknown>): boolean;
  describe(): string; // human-readable, e.g. "(country == IN AND amount > 1000)"
}
```

**Task:**
- Implement all four node types so `evaluate()` recurses through the tree.
- `describe()` must produce a readable, parenthesized expression by recursing.
- Support **short-circuit evaluation** (`AndNode` stops at the first `false`; `OrNode` stops at the first `true`).

**Think about:** `NotNode` is a composite with exactly ONE child — how does that blur the line with the Decorator pattern? Where is the base case?

---

## Real-World Challenge — Storage Service with Persistence & Caching

Extend the file-system example from [code.ts](code.ts) toward production.

**Requirements:**
1. **Persistence design (write it up, then implement one).** Describe how you would store the tree in PostgreSQL using each of: adjacency list (`parent_id`), materialized path, and nested sets. Pick one and implement a `TreeRepository` that loads a subtree into `FileSystemNode` objects and saves changes. (Mock the DB with an in-memory map; no real Postgres needed.)
2. **Denormalized aggregate.** Add a `total_size` field to each directory record that is kept up to date on write, so `getSizeInBytes()` can be answered from storage **without** walking the subtree. Keep it consistent when a file is added, removed, or moved.
3. **Cycle prevention.** Make `add()` reject any operation that would place a directory inside its own subtree (directly or transitively). Add a test that proves it throws.
4. **Depth safety.** Add a `move(node, newParent)` operation and an iterative "delete entire subtree" that does not recurse on the call stack. Verify it works on a tree 20,000 nodes deep without a stack overflow.
5. **Client decoupling test.** Write a unit test for a `QuotaService` using a tiny hand-built in-memory tree and **no repository/DB at all** — proving the client depends only on the `FileSystemNode` interface.

**Stretch:** Add a `presignAllUrls()` operation. Implement it once as a new interface method (feel the pain of editing every node class), then re-implement it as a **Visitor** and compare the diffs. Which extensibility axis did each approach optimize?

---

## Bonus Challenge — Composite + Visitor + Iterator Together

1. **Visitor.** Define a `FileSystemVisitor<R>` and implement three visitors over the file tree without editing any node class: `TotalSizeVisitor`, `FileTypeHistogramVisitor` (count files per content-type), and `DeepestPathVisitor` (find the longest path). Explain why Visitor solves Composite's "adding an operation touches every class" weakness.

2. **Iterator.** Implement both a **depth-first** and a **breadth-first** iterator over the tree as JavaScript generators. Then implement a `find(predicate)` that stops early when the first match is found — prove it does not visit the whole tree.

3. **Transparency experiment.** Refactor your tree once in the *transparent* style (put `add`/`remove` on the shared interface; leaves throw `NotSupportedError`) and once in the *safe* style (composite-only). Write down, for each: what the client code looks like, what breaks at compile time vs runtime, and which you would ship. Reference the transparency-vs-safety discussion in the README.

4. **Flyweight combo.** Suppose your tree has 1,000,000 leaves that are identical "empty placeholder" files. Show how sharing a single flyweight leaf instance across the tree changes memory usage, and what invariant that requires (hint: shared leaves must be immutable).

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
