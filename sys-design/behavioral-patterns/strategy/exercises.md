# Strategy Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting a *behavior-selecting conditional* and replacing it with interchangeable algorithms behind one interface.

> Rule of thumb for every exercise: design the **Strategy interface** from the *task*, not from one algorithm's needs. The **Context** must only *hold and delegate* — it contains no `if (type === ...)` selection. Keep strategies **stateless**, and put the *choosing* logic in a **registry/factory/map**, never in the Context.

---

## Easy — Discount Strategy

Your checkout applies a discount to an order total. Today there are three:

```ts
// Total is in integer cents.
function applyDiscount(kind: string, totalCents: number): number {
  if (kind === "none") return totalCents;
  else if (kind === "percent10") return Math.round(totalCents * 0.9);
  else if (kind === "flat500") return Math.max(0, totalCents - 500);
  // ...and marketing keeps asking for more
}
```

**Task:** Refactor this into a `DiscountStrategy` interface with a single `apply(totalCents): number` method, and three concrete strategies (`NoDiscount`, `PercentOff`, `FlatOff`). Write a tiny `Checkout` Context that holds a `DiscountStrategy` and delegates. Prove a `DiscountStrategy`-typed variable can hold any of the three.

**Acceptance:** For a $50.00 order, `NoDiscount` → 5000, `PercentOff(10)` → 4500, `FlatOff(500)` → 4500. The `Checkout` Context contains **no** `if`/`switch` selecting the discount.

---

## Medium — Functions as Strategies + a Registry

Reuse the discount idea, but this time:

1. Express each discount as a **plain function** of type `(totalCents: number) => number` — no classes.
2. Put them in a `Record<string, DiscountFn>` map.
3. Add a `resolveDiscount(code: string)` selector that looks up the map and throws a clear `UnknownDiscountError` for an unknown code.

**Task:** Show that adding a new discount (`"buyOneGetHalf"`) means adding **one entry** to the map — no existing function or the selector changes. Then show the class-based version and the function-based version producing identical results for the same inputs.

**Bonus constraint:** Make `PercentOff` configurable (the percentage is injected) in the class version, and explain in a comment why the *function* version is the more idiomatic TS choice when there is no injected state.

---

## Hard — Ranking Strategies for a Feed

A social feed must order posts by a selectable algorithm:

```ts
interface Post {
  id: string;
  createdAt: number;   // epoch ms
  likes: number;
  authorFollowedByViewer: boolean;
}
```

You must support three ranking algorithms, chosen at runtime from a query param:
1. `"newest"` — most recent first.
2. `"top"` — highest likes first, ties broken by newest.
3. `"relevant"` — a score combining recency, likes, and whether the viewer follows the author.

**Task:**
- Define a `RankingStrategy` interface with `rank(posts: Post[]): Post[]` (return a *new* sorted array; do not mutate the input).
- Implement the three strategies, each stateless.
- Build a `FeedService` Context that holds a `RankingStrategy` and exposes `getFeed(posts)`.
- Select the strategy via a registry keyed by the query param, defaulting sensibly for an unknown value.

**Think about:** Where does the tie-breaking rule live — the interface or each strategy? Why must `rank` be pure (no mutation, no shared state) for a single strategy instance to be safely shared across concurrent requests?

---

## Real-World Challenge — Pluggable Password Hashing (Seamless Migration)

Your auth service must hash and verify passwords, and you need to migrate from `bcrypt` to `argon2` **without a big-bang rewrite** — old hashes must keep verifying while new hashes use the new algorithm.

Design a `HashStrategy` port used by a NestJS-style service:

```ts
interface HashStrategy {
  readonly id: "bcrypt" | "argon2";        // stored alongside the hash
  hash(plain: string): Promise<string>;
  verify(plain: string, hash: string): Promise<boolean>;
}
```

Provide **two** strategies against simulated crypto libs (mock the actual hashing — no real crypto calls needed):
1. `BcryptStrategy`
2. `Argon2Strategy`

**Requirements:**
- The stored credential is `{ algoId, hash }`. On **verify**, resolve the strategy by the stored `algoId` (a registry keyed by `id`), so old bcrypt hashes verify with bcrypt and new ones with argon2.
- On **create/update**, always use the *current default* strategy (argon2), selected at a single composition root — flipping the default is a one-line change.
- The `AuthService` (Context/consumer) depends only on `HashStrategy` and the registry; it contains **no** `if (algo === "bcrypt")`.
- Add a `needsRehash(credential)` helper that returns true when a stored credential's algo is not the current default — so you can transparently upgrade a hash on next successful login.

**Stretch:** Add a third strategy (`Scrypt`) and confirm you modified **no** existing strategy and **no** consumer code — only added a file and one registry entry. Write down which SOLID principles this demonstrates.

---

## Bonus Challenge — Strategy vs State, and Selection Without a Switch

1. **Strategy vs State — prove you can tell them apart.** Take the ranking feed from the Hard exercise (Strategy: the *client* picks the algorithm, algorithms don't know about each other) and contrast it with a small **State** example — e.g. a `Document` that moves `Draft → Moderation → Published`, where each state decides the *next* state and triggers the transition itself. In a written comment, list the exact differences: who chooses the object, whether the objects know about each other, and the intent (interchangeable algorithms vs lifecycle transitions).

2. **Strategy vs Template Method.** Reimplement one discount using **Template Method** instead: a base `abstract class DiscountBase` with a fixed `apply()` skeleton (validate → compute → clamp to ≥ 0) and an abstract `compute(totalCents)` step subclasses override. Then contrast it with the composition-based Strategy version. Explain when the fixed-skeleton/inheritance approach is the better fit and when it locks you in (compile-time selection, single-inheritance slot).

3. **Kill the growing switch.** Start from a deliberately bad `getQuote` that has a `switch (method)` inside the service. Refactor the selection out into a registry/factory map so the service becomes pure delegation. Then add a brand-new method (`"same-day"`) and confirm the service and every existing strategy stay **byte-for-byte unchanged** — only a new strategy file and one `register(...)` line are added. Name the principle (Open/Closed) and explain where the `switch` "went".

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
