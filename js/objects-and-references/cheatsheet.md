# Objects and References — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | JS core language semantics (reference model, copying, comparison). |
| **Core Idea** | Variables never hold objects — they hold **references** to heap objects. Primitives are copied by value; objects/arrays/functions are shared as references. |
| **Reference Semantics** | `const b = a` copies the *reference*, not the object. `a === b` is `true`; mutating through either name is visible through both. `{} === {}` is always `false`. |
| **Mutation vs Reassignment** | **Mutate** (`obj.x = 1`, `arr.push`) → follows the reference, visible everywhere. **Reassign** (`obj = ...`) → rebinds only the local variable; callers never see it. `const` locks the binding, not the object. |
| **Shallow vs Deep Copy** | Shallow: `{...obj}`, `Object.assign({}, obj)` — top level new, nested objects still shared. Deep: `structuredClone(obj)` (modern; handles Dates/cycles). JSON round-trip: deep but drops functions/undefined/Symbols, mangles NaN→null and Date→string, throws on cycles. Freeze/spread are also shallow. |
| **Comparison** | `===` on objects compares references only. No built-in structural equality — write your own `isEqual` or use a library (`_.isEqual`). Use `Object.is` for primitives when you need `NaN === NaN`. |
| **Gotchas** | Function args are copies of the reference → mutate yes, reassign no · `{...state}` then editing `state.nested` corrupts the original · `sort/splice/push/reverse` mutate in place; `map/filter/slice/toSorted` don't · `Object.freeze` is one level deep · JSON round-trip silently loses types. |
| **Use When** | Immutable updates in React/Redux (rebuild every level of the mutated path) · defensive copies of inputs · returning fresh state from reducers · caching/cloning config objects. |
| **Related Topics** | Immutability patterns · garbage collection & the heap · pass-by-sharing vs pass-by-reference · property descriptors / freeze/seal · structuredClone vs serialization libraries · prototype chains. |

### Skeleton
```js
const b = a;            // alias — same object, NOT a copy
const shallow = { ...a };        // 1 level deep
const deep     = structuredClone(a); // fully independent
function f(o) { o.x = 1; }       // caller sees this (mutation)
function g(o) { o = {};   }      // caller does NOT (reassignment)
```

### Remember In One Sentence
> **A variable holds an arrow to an object, not the object itself — so mutation travels through every alias, while reassignment changes only the arrow you point at.**

### Two Facts People Get Wrong
- "`const` makes objects immutable? No" — it freezes the *binding*; contents stay mutable until you also `Object.freeze` (which is itself shallow).
- "`JSON.parse(JSON.stringify(x))` is a safe deep copy? No" — it silently drops functions/`undefined`, converts `Date` to string and `NaN` to `null`, and throws on circular references.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README.

1. What exactly does `const b = { ... a }` copy when `a` contains a nested object — and what happens when you mutate `b.nested.x`?
2. Explain why `rename2`'s `obj = { ... }` inside a function doesn't affect the caller's variable, but `obj.name = "..."` does.
3. What does JS actually pass to functions — the object, a reference, or something else? Name the term.
4. Give three things `JSON.parse(JSON.stringify())` mishandles and what each becomes.
5. Why is `structuredClone` preferred over the JSON round-trip? Name two things it still cannot carry over.
6. What does `===` compare for two objects with identical content? How would you check content equality?
7. Is `Object.freeze({ list: [] })` enough to stop `obj.list.push(1)`? Why or why not?
8. Which array methods mutate in place and which return new arrays? Give three of each from memory.
