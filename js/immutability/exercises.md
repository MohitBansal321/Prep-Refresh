# Immutability — Exercises

Work through these in order. Do not look at any solution; the goal is to build two reflexes: (1) spotting every place a mutation can leak, and (2) producing new state by *copying the path, sharing the rest*.

> Rule of thumb for every exercise: after your code runs, `console.log` the ORIGINAL input. If it changed and you didn't intend that, you have a leak.

---

## Easy — Predict the Leaks

For each snippet below, write down (before running) what will be logged:

1. `const a = [1, 2, 3]; const b = a; b.push(4);` — what is `a.length`?
2. `const x = { n: { m: 1 } }; const y = { ...x }; y.n.m = 2;` — what is `x.n.m`?
3. `const s = "hi"; s.toUpperCase();` — what is `s`?
4. `const f = Object.freeze({ arr: [1] }); f.arr.push(2);` — does this throw? What is `f.arr`?

**Acceptance:** run each in node; compare with your prediction. For every miss, explain *which reference* was shared or which operation mutates.

**Then answer in a comment:** why do #2 and #4 fail even though they look like they "copied" or "protected" the data?

---

## Easy — Immutable Shopping Cart

Given `const cart = [{ item: "pen", qty: 1 }, { item: "book", qty: 2 }]`, produce functions (without mutating `cart` or its elements):

- `addItem(cart, item)` — returns cart + one more entry
- `changeQty(cart, itemName, delta)` — returns new cart where matching item's qty changed
- `removeItem(cart, itemName)` — returns cart without that item

**Acceptance:** after all three calls on the same starting cart, the original `cart` still deep-equals its initial value (verify with `structuredClone(cart)` taken at the start and a comparison at the end).

---

## Medium — deepFreeze(obj)

Implement `deepFreeze(obj)` without looking anything up beyond syntax.

**Requirements:**
- Freezes `obj` and every nested object/array reachable from it.
- Handles arrays as well as objects.
- Returns the object (so calls can be chained/used inline).
- Cycles must not cause infinite recursion (track visited objects).

**Acceptance:**
```js
const cfg = deepFreeze({ a: { b: [1, { c: 2 }] } });
cfg.a.b[0] = 99;          // no effect (non-strict) or throws (strict)
// cfg.a.b[0] is still 1
```

**Think about:** when is deep-freezing a BAD idea for app state (hint: think about how often state updates and what freeze costs)?

---

## Medium — updateIn(state, path, value)

Implement `updateIn(obj, ["user", "address", "city"], "Mumbai")` that returns a new object with only the levels along `path` copied.

**Requirements:**
- `path` is an array of keys (support array indices too, e.g. `["posts", 0]`).
- Untouched branches must be **reference-equal** to the original (`newState.posts === state.posts` stays true).
- The original object must be untouched.
- Bonus: support a function instead of a value — `updateIn(obj, path, v => v + 1)`.

**Acceptance:**
- Original unchanged after the call.
- Changed branch has fresh references at each level of the path.
- At least one untouched sibling branch shares its old reference.

**Then answer in a comment:** why does copying ONLY the touched path (instead of deep-cloning everything) matter for React re-render performance?

---

## Hard — Immutable Undo Stack

Build a text-editor-like history manager WITHOUT classes if you can (plain closures are fine):

**Requirements:**
- `createHistory(initialText)` → API `{ type(str), undo(), redo(), get() }`.
- Every operation returns a NEW history object; the previous one remains valid and usable (this is what makes "time travel" possible).
- `undo()` after `type()` restores the prior text; `redo()` reapplies it. A `type()` after an `undo()` discards the redo branch (standard editor behavior).

**Acceptance:**
```js
const h0 = createHistory("a");
const h1 = h0.type("b");
const h2 = h1.type("c");
h2.get();      // "abc"
h1.get();      // "ab"   <- old version still alive
h0.undo().get(); // ""   <- works independently
```

**Think about:** why would this be impossible if `type()` mutated shared state? Where exactly does your implementation rely on immutability?

---

## Self-check before moving on

- [ ] I can list the mutating array methods from memory and their non-mutating twins.
- [ ] I can immutably update a 3-level nested property without pausing to think.
- [ ] I understand why `===` between two states proves "nothing changed" only under immutability.
- [ ] I can explain shallow copy vs deep copy vs freeze in three sentences.
