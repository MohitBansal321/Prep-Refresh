# `this` and Prototypes — Exercises

Work through these in order. There are **no solutions here** — the goal is to build three reflexes: (1) predicting what `this` is by reading only the call site, (2) implementing context control (`call`/`apply`/`bind`) from scratch, and (3) seeing that `class` and constructor-function inheritance produce the same prototype structure.

Run your work with plain Node (e.g. `node myAttempt.js`). Verify claims about `this` by printing it — but log a `name` property or a boolean, never a raw object dump.

---

## Easy — Predict-the-Output Puzzles

Write five small snippets, and **before running them**, write down on paper what each will print and which of the 5 binding rules applies.

**Requirements:**
- Snippet A: an object method called normally, then the same method extracted to a variable and called — both under strict mode.
- Snippet B: the same extracted call again inside a **non-strict** function created via `new Function(...)` — observe how default binding changes.
- Snippet C: a nested regular function and a nested arrow function inside a method; print `this.name` in each.
- Snippet D: an arrow defined at the top level of a CommonJS module, invoked via `.call(someObj)` — show its `this` does not change.
- Snippet E: a hard-bound function re-bound with `.bind` a second time and then called with `.call` using yet another context.

**Acceptance:**
- For every snippet you predicted correctly, note *which rule* decided it in one line.
- Every wrong prediction gets a written explanation naming the exact mechanism you misjudged.

**Reflection:** Which single rule caused most of your wrong predictions? What visual cue at the call site would have caught it?

---

## Medium — Implement `myBind(myFn, ctx)` Without Built-in `bind`

Build `myBind(fn, ctx, ...presetArgs)` that returns a new function behaving exactly like `fn.bind(ctx, ...presetArgs)`.

**Requirements:**
- The returned function calls `fn` with `ctx` as `this`, prepending any preset ("partial application") arguments to whatever arguments the caller passes later.
- Re-binding or `.call`ing the returned function must NOT change its context (hard binding).
- Bonus correctness points: make the bound function work correctly when invoked with `new` — i.e., `new myBound()` should ignore `ctx`, construct a fresh object, and pass `instanceof` checks against the original constructor. (Research how `bind` treats functions used as constructors before attempting this.)

**Acceptance:**
- With `const obj = { name: 'bound' }` and `function greet(x, y) { return this.name + x + y }`: `myBind(greet, obj, 'a')('b')` behaves identically to `greet.bind(obj, 'a')('b')`.
- `myBound.call(otherCtx, ...)` still uses `ctx` — prove it with two different contexts.
- If you attempted the bonus: `new myBound() instanceof greet` is `true`.

**Reflection:** Why is the classic naive implementation (assigning `fn` onto `ctx` as a temporary property and calling it) considered acceptable here, and what subtle problem does that technique have if `ctx` already has a property with that name?

---

## Hard — Inheritance Chain Two Ways

Model `Animal → Dog → Puppy` twice: once with constructor functions + prototypes, once with `class`. Then compare them structurally.

**Part 1 — Constructor functions:**
- `Animal(name)` stores `name`; `Animal.prototype.describe()` returns `"<name> is an animal"`.
- `Dog(name, breed)` reuses Animal's initialization and adds `breed`; `Dog.prototype.fetch()` returns `"<name> fetches!"`.
- `Puppy(name, breed)` chains up one more level; `Puppy.prototype.describe()` overrides the inherited one but includes the parent's text plus `" (and a puppy)"`.
- Remember all three steps of wiring the chain — including repairing the `constructor` pointer.

**Part 2 — Classes:**
- Rewrite the identical hierarchy with `class`/`extends`/`super`.
- Do NOT copy-paste behavior blindly: use `super` where the old code called the parent constructor or the parent's method.

**Part 3 — Structural comparison:**
- Prove the two hierarchies are the same shape: for the class version, verify `puppy instanceof Dog && puppy instanceof Animal && puppy instanceof Object`, and that instance methods still live on `ClassName.prototype` (compare `aMethod === ClassName.prototype.method` across two instances).
- Show shadowing in action: give a puppy its own `name = 'Spot'` after construction, confirm `describe()` now uses it, then delete the own property and show the original returns.

**Acceptance:**
- Both versions produce identical output for the same construction sequence.
- `Dog.prototype.constructor === Dog` is `true` in the constructor-function version (the classic forgotten step).

**Reflection:** List two bugs that the `class` syntax makes impossible which the constructor-function version allows (hint: one involves forgetting `new`, one involves the `constructor` pointer). Which version would you defend in an interview as "the real JavaScript model", and why is the honest answer "they are the same model"?

---

## Real-World Challenge — Fix a Broken Event Handler Module

You inherit this pattern (reproduce it yourself first, watch it fail):

```js
const cart = {
  items: [],
  label: 'CartModule',
  addItem(item) { this.items.push(item); },
};
```

Somewhere else, `cart.addItem` is passed directly as an event/callback handler (simulate with a fake event bus that stores callbacks and invokes them receiver-less later). It breaks.

**Task:**
1. Reproduce the failure and explain in writing which binding rule fires instead of implicit binding, and why.
2. Fix it three different ways — arrow wrapper at registration, `.bind`, and converting `addItem` into a class-field arrow on a class — and state one tradeoff of each (e.g., testability, memory per instance, readability).
3. Make the failure loud: wrap the original broken version so a lost `this` throws immediately with a helpful message instead of silently corrupting state (think: what does strict mode already do for you here, and what gap remains?).
4. Explain whether spreading `...args` through your fix preserves argument order, and why partial application interacts with handler signatures.

**Acceptance:**
- The fake event bus invokes the fixed handler and `cart.items` gains the item.
- You can articulate, in one sentence each, why each of the three fixes works in terms of the 5 rules.

**Reflection:** If `addItem` needed to be unit-tested without a real event bus, which of the three fixes makes testing easiest and which makes it hardest? Why?
