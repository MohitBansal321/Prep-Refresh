# Objects and References — Exercises

Work through these in order. Do not look for a solution; the goal is to build two reflexes: (1) predicting mutation/aliasing behavior *before* running code, and (2) reaching for the right copy strategy without thinking.

> Rule of thumb for every exercise: predict every `console.log` output **on paper first**, then run and compare. A wrong prediction is worth more than a right one you guessed after seeing the output.

---

## Easy — Predict the Output: Alias or Copy?

Write this script yourself (do not copy from `code.js`), predicting each line's output in a comment *before* running it:

```js
const a = { list: [1, 2], n: 1 };
const b = a;
const c = { ...a };

b.n = 2;
c.n = 3;
b.list.push(99);

console.log(a, b, c);
console.log(a === b, a === c, a.list === c.list);
```

**Requirements:**
- Every `console.log` gets its predicted output as a trailing comment, written before running.
- After running, correct any wrong predictions and add a one-line note on what you got wrong.

**Acceptance:**
- You can explain, in one sentence per line, *which* of the three variables share references at each step.
- Final state of `a`, `b`, `c` matches your (corrected) prediction exactly.

**Then answer in a comment:** which single word describes what `{ ...a }` copied into `c.list` — value, reference, or object?

---

## Medium — Write Your Own deepClone

Implement `deepClone(value)` without using `structuredClone` or JSON.

**Requirements:**
- Handles nested plain objects and arrays at any depth.
- Primitives are returned as-is; `Date` instances come back as real, independent `Date`s.
- Handles circular references without infinite recursion (hint: track visited objects in a `Map`).
- Does not need to handle functions, `Map`/`Set`, getters, or class prototypes — but add a comment noting that it doesn't.

**Acceptance:**
- `deepClone({ a: { b: [1, { c: 2 }] } })` produces a structurally equal result where **no** inner object/array is reference-equal (`!==`) to the original's.
- Mutating any part of the clone never mutates the original (verify with asserts).
- Cloning a cyclic object terminates and yields a correctly cyclic clone.
- Compare your result against `structuredClone` for the same input — same shape.

**Think about:** why does a naive recursive clone explode on cycles? At exactly what point does your `Map` check prevent it — and would an array work instead of a `Map` there?

---

## Hard — Implement isEqual for Nested Structures

Implement `isEqual(a, b)` — deep structural equality, no libraries.

**Requirements:**
- Primitives compared with `Object.is` (so `NaN` equals `NaN`; explain why `===` fails here).
- Objects equal only if same set of keys **and** every value is recursively equal (order-independent for objects).
- Arrays equal only if same length and element-wise order matches.
- `null` vs `{}` must be `false`; distinguish arrays from plain objects.
- Handle self-referential pairs gracefully (two distinct objects that both reference themselves compare equal, like `structuredClone`'s output would).

**Acceptance:**
- `isEqual({ x: [1, NaN] }, { x: [1, NaN] })` → `true`.
- `isEqual({ a: 1, b: 2 }, { b: 2, a: 1 })` → `true`.
- `isEqual([1, 2], [2, 1])` → `false`.
- `isEqual(null, {})` → `false` and `isEqual({}, [])` → `false`.
- All checks via `assert`, self-verifying when run with node.

**Then answer in a comment:** why does checking `Object.keys(a).length === Object.keys(b).length` matter before looping keys — give an example where skipping it gives a wrong `true`.

---

## Bonus — The State Update Gauntlet

You have this state tree (React-style immutable updates):

```js
const state = {
    cart: {
        items: [
            { id: "p1", qty: 1, meta: { giftWrap: false } },
            { id: "p2", qty: 3, meta: { giftWrap: true } },
        ],
        coupon: null,
    },
    user: { name: "Mohit" },
};
```

**Task:** write four functions, each returning a NEW state where:
1. `p1`'s `qty` becomes 5 — nothing else changes, original untouched.
2. `p2`'s `giftWrap` flips to `false`.
3. `coupon` becomes `"SAVE10"`.
4. Item `p2` is removed entirely.

**Constraints:** no mutation anywhere (no `push`, no property assignment on anything reachable from `state`); use spread/rest only. Add assertions proving the ORIGINAL `state` is byte-for-byte unchanged after all four calls (deep-compare against a snapshot taken up front).

**Think about:** for update #1, exactly how many levels did you rebuild, and why is rebuilding more levels than necessary wasteful — while rebuilding fewer breaks immutability?
