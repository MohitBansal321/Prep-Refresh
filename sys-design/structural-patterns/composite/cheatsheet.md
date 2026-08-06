# Composite Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Structural design pattern (GoF). |
| **Intent** | Compose objects into tree structures for part-whole hierarchies, and let clients treat individual objects (leaves) and compositions (composites) uniformly through one interface. |
| **Problem** | You have tree-shaped data (files/dirs, org charts, categories, rule trees) and need operations that recurse to arbitrary depth — without the client writing `instanceof` ladders and re-implementing traversal for every operation. |
| **Solution** | Define a Component interface both leaves and composites implement. Leaves answer directly (base case); composites delegate the same call to each child and aggregate (recursive step). The client calls one uniform method. |
| **Participants** | **Component** (shared interface) · **Leaf** (no children, base case) · **Composite** (holds children, delegates + aggregates) · **Client** (uses Component, no type checks). |
| **Flow** | Client → calls Component method on a node → if leaf, return own value → if composite, loop children calling the same method → aggregate results → return up the tree. |
| **Pros** | Uniform treatment of leaves & groups · recursion encapsulated in the structure · Open/Closed for new node types · models real hierarchies naturally · pairs with Visitor/Iterator. |
| **Cons** | Interface tends to lowest-common-denominator · transparency/safety compromise · recursion-depth risk · aggregate caching/invalidation complexity · cycle prevention needed · adding an *operation* touches every class (unless Visitor). |
| **Use When** | Part-whole hierarchies (file trees, org charts, category/menu trees, DOM/UI, BOMs, permission/rule trees) · clients must treat items & groups uniformly · operations recurse to unknown depth. |
| **Avoid When** | Data is flat or a cyclic graph, not a tree · nesting is one fixed level (YAGNI) · leaves & composites share no meaningful operations · huge tree lives only in the DB and needs no rich in-memory behavior. |
| **Real Examples** | React/DOM component tree · Express nested routers · AWS CDK construct tree & Organizations OUs · Azure Mgmt-Group→Subscription→RG hierarchy · GCP Org→Folder→Project · file systems · ASTs / behavior trees. |
| **Related Topics** | Decorator (wrap ONE, add behavior) · Visitor (add operations without editing nodes) · Iterator (traverse) · Flyweight (share identical leaves) · Chain of Responsibility · SQL tree storage (adjacency list / materialized path / nested sets / recursive CTE). |

### Transparency vs Safety
- **Transparent** — `add`/`remove` on the **Component** interface. Leaves & composites look identical; calling `add()` on a leaf is a *runtime* error. Max uniformity.
- **Safe** — `add`/`remove` **only on the Composite**. Calling `add()` on a leaf is a *compile* error. Preferred in TypeScript (our `code.ts` uses this).

### Skeleton
```ts
interface Component {                              // shared contract
  operation(): number;
}
class Leaf implements Component {                  // base case
  operation(): number { return this.value; }
}
class Composite implements Component {             // recursive step
  private children: Component[] = [];
  add(c: Component) { this.children.push(c); }     // safety: composite-only
  operation(): number {
    return this.children.reduce((sum, c) => sum + c.operation(), 0); // delegate + aggregate
  }
}
```

### Remember In One Sentence
> **A Composite lets you call the same method on a single leaf or a whole tree of them and get a correct answer — because the tree knows how to recurse through itself, so the client never has to.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the four participants and give each a one-line responsibility.
2. Which participant is the recursion's *base case* and which is the *recursive step*? Why?
3. What is the transparency-vs-safety tradeoff, and which does TypeScript favor?
4. Why must a composite hold its children typed as the *Component interface* rather than concrete types?
5. Composite makes adding a new *type* cheap but adding a new *operation* expensive — which pattern flips that, and how?
6. How do you compute recursive aggregates (like total size) efficiently, and what must you do on mutation?
7. Name two failure modes on large/deep trees and how you defend against each.
8. How do you prevent cycles in the tree, and why does it matter for traversal?
9. Name the three classic ways to store a tree in a relational database and their read/write tradeoff.
10. How is Composite different from Decorator, given both use recursive composition?
