# Immutability

**Prerequisite:** read [js/objects-and-references](../objects-and-references/README.md) first — immutability only makes sense once you're comfortable with references.

## 1. What immutability is

Immutable means *"cannot be changed."* In JavaScript:

- **Primitive values are always immutable.** You cannot alter `"hello"` or `42` — you can only create new values and point variables at them.
- **Objects and arrays are mutable by default.** Unless you take deliberate action, any code holding a reference can change what's inside.

```js
let s = "hello";
s[0] = "H";        // silently ignored
console.log(s);    // "hello"

const arr = [1, 2, 3];
arr[0] = 99;       // works fine — arrays are mutable
console.log(arr);  // [99, 2, 3]
```

Note that `const` does **not** give you immutability. It locks the *binding* (which value the variable points to), not the *value itself*:

```js
const obj = { name: "Mohit" };
obj.name = "John";              // fine — mutating, not reassigning
obj = {};                       // TypeError — reassigning a const binding
```

## 2. Why immutability matters

1. **Predictability** — a value that can't change behaves the same everywhere it's passed.
2. **Change detection by reference** — if state never mutates, checking "did anything change?" becomes `oldState === newState`. This is exactly how React decides to re-render: new object reference → something changed → render.
3. **Time-travel debugging / undo** — because old states are never destroyed, you can keep a history of them.
4. **Safe sharing** — passing an object to a function can't secretly corrupt your copy if neither of you mutates.

## 3. The mutation traps

### Shared references leak mutations

```js
const original = { user: { name: "Mohit" } };
const copy = { ...original };      // SHALLOW copy
copy.user.name = "HACKED";
console.log(original.user.name);   // "HACKED" — inner object was shared!
```

Spread, `Object.assign`, `Array.prototype.slice`, `[...arr]` are all **shallow**: one level deep only.

### Some array methods mutate in place

`push`, `pop`, `shift`, `unshift`, `splice`, `sort`, `reverse`, `fill`, `copyWithin` all modify the array you call them on. Calling `state.items.sort()` in a reducer mutates your previous state — a classic bug source.

## 4. Copy strategies

| Strategy | Depth | Notes |
|---|---|---|
| `{ ...obj }` / `[...arr]` | shallow | cheap; shares nested values |
| `Object.assign({}, obj)` | shallow | same as spread |
| `structuredClone(obj)` | deep | built-in; handles Dates, Maps, circular refs; no functions |
| `JSON.parse(JSON.stringify(o))` | deep-ish | loses functions, `undefined`, Dates→strings; last resort |
| `deepFreeze(obj)` | n/a | prevents mutation instead of copying |

## 5. Immutable update patterns (the interview core skill)

Update a nested structure **without touching anything** along the way — spread a fresh object at *each level on the path*, reuse everything else:

```js
const state = {
  user: { name: "Mohit", address: { city: "Delhi" } },
  posts: ["p1", "p2"],
};

const newState = {
  ...state,                                        // new top level
  user: {
    ...state.user,                                 // changed branch copied
    address: { ...state.user.address, city: "Mumbai" },
  },
};
```

- `newState.user.address.city` → `"Mumbai"`
- `state` still says `"Delhi"` — history preserved
- Untouched branches (`posts`) are **shared**, so this stays cheap

For arrays, prefer the non-mutating twins:

```js
list = [...list, item];                 // append
list.filter(x => x !== target);         // remove
list.map(x => x === target ? { ...x, done: true } : x); // update element
[...nums].sort((a, b) => a - b);        // sorted copy
nums.toSorted(...), nums.toReversed(), nums.with(i, v)  // ES2023 built-ins
```

## 6. Object.freeze and its limits

`Object.freeze(obj)` makes top-level properties read-only (in strict mode writes throw; otherwise they're silent no-ops). But it is **shallow** — nested objects stay editable:

```js
const f = Object.freeze({ inner: { x: 10 } });
f.inner.x = 99;
console.log(f.inner.x);   // 99 — freeze didn't reach inside
```

A recursive `deepFreeze` walks the tree freezing each level (write it as an exercise below). Freeze is for *guarding constants/config*; for frequently-updated app state you'd use a library like **Immer** (`produce(state, draft => {...})` gives immutable results with mutable-looking code).

## Key Takeaways

- `const` ≠ immutability — it only stops reassignment.
- Primitives are always immutable; objects/arrays need deliberate effort.
- All common copies (`{...}`, `slice`, `assign`) are shallow — nested mutation leaks through.
- Immutable update rule: **copy every level you touch, share everything else.**
- Know which array methods mutate; reach for `map/filter/slice/toSorted` instead.
- Reference equality (`a === b`) doubles as change detection only under immutability.

## Common Interview Questions

1. Is JavaScript pass-by-value or pass-by-reference? How does that interact with immutability?
2. What does `const` actually guarantee? Show code where a const object changes.
3. Why does React require immutable state updates? What breaks if you mutate?
4. Shallow vs deep copy — give three ways to deep-copy and their trade-offs.
5. Which array methods mutate their receiver? Name the non-mutating alternatives (incl. ES2023).
6. Write the pattern to immutably update `state.a.b.c`.
7. What does `Object.freeze` do and where does it fall short?
8. When would immutability hurt performance, and how do libraries like Immer/structural sharing help?
