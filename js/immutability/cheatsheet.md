# Immutability — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core JS discipline / state-management pattern. |
| **Core Idea** | Never change existing objects/arrays — produce a **new** value for every change; old values stay valid forever. |
| **Why It Matters** | Predictability · change detection via `===` (React re-render model) · undo/time-travel · safe sharing across functions/threads of control. |
| **Primitives vs Objects** | Primitives are *always* immutable. Objects/arrays are mutable by default; `const` stops **reassignment**, not mutation. |
| **Mutating Methods To Avoid** | `push pop shift unshift splice sort reverse fill copyWithin` (+ assigning to indexes/properties). |
| **Non-Mutating Alternatives** | `map filter slice concat flat flatMap`, ES2023 twins `toSorted toReversed toSpliced with`, spread `[...arr, x]`. |
| **Copy Strategies** | `{...obj}` / `[...arr]` = shallow · `structuredClone` = deep (no functions) · `JSON` round-trip = lossy last resort. |
| **Object.freeze Limits** | Shallow! Nested objects stay writable; non-strict writes are silent no-ops. Need recursive `deepFreeze`. |
| **Nested Update Pattern** | Copy every level **on the path**: `{...state, user: {...state.user, address: {...state.user.address, city}}}` — untouched branches stay reference-equal (cheap). |
| **Gotchas** | All common copies are shallow · `sort()` mutates in place · freeze ≠ deep · Immer-style libraries give immutable results from mutable-looking code. |
| **Related Topics** | [objects-and-references](../objects-and-references/README.md) (reference model) · arrays-and-array-methods · scope-and-closures. |

### Skeleton
```js
const next = {                       // new top level
  ...state,
  user: {
    ...state.user,                   // copy each level you touch...
    address: { ...state.user.address, city: "Mumbai" },
  },
};
// untouched branches shared by reference => fast + old state intact
```

### Remember In One Sentence
> **Immutability means every change produces a new object — copy the path you touch, share everything you don't — so that reference equality itself becomes proof that nothing changed.**

### Two Facts People Get Wrong
- `const obj = {...}; obj.x = 1` is legal — `const` freezes the *binding*, not the *value*.
- `Object.freeze` and `{...spread}` are both one level deep; nested data leaks mutations either way.

---

## Recall Questions (answer from memory — no peeking)

1. What exactly does `const` guarantee, and what does it NOT guarantee?
2. Why are primitives always immutable while objects aren't? What happens internally when you "modify" a string?
3. List five mutating array methods and their non-mutating replacements.
4. What does "shallow copy" mean, and why does `{...state}` fail for nested updates?
5. State the rule for immutably updating a deeply-nested property — which levels get copied and which are shared?
6. How does React use reference equality to detect changes, and why does mutation break it?
7. `Object.freeze({ inner: {} })` — can `inner` still be mutated? How do you fix it?
8. Name three ways to deep-copy an object and one limitation of each.
