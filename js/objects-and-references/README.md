# Objects and References — The Mental Model That Fixes Half of JS Interview Questions

If you internalize one thing from the JS section, make it this: **variables never hold objects. They hold references (pointers) to objects.** Almost every "wait, why did THAT change?" bug — and a huge share of interview trick questions — comes from forgetting this.

---

## 1. Objects Are Reference Types

Primitives (`number`, `string`, `boolean`, `null`, `undefined`, `symbol`, `bigint`) are stored **by value**. Objects (plain objects, arrays, functions, `Date`, `Map`, `Set`...) are stored **by reference**.

```js
let a = 10;
let b = a;      // b gets a COPY of the value 10
b = 20;
console.log(a); // 10 — untouched

let objA = { name: "Mohit" };
let objB = objA;            // objB gets a COPY OF THE REFERENCE, not the object
objB.name = "Changed";
console.log(objA.name);     // "Changed" — same object, seen through two names
```

There is only **one object** in memory. `objA` and `objB` are two arrows pointing at it. Mutating through either arrow is visible through both.

## 2. The Reference Model: Variables Hold Pointers to the Heap

When you write `const user = { name: "Mohit" }`, two things happen:

1. An object is created **on the heap** (the garbage-collected memory area).
2. The variable `user` holds a small **reference** to that heap location — like an address.

```js
const x = { id: 1 };
const y = { id: 1 };

x === y;          // false! Two separate objects on the heap
const z = x;
z === x;          // true — same reference
```

Think of it as house addresses: `x` and `y` are two *identical twins' houses* (different addresses), while `z` is the *same house as* `x` written on a different piece of paper.

Key consequence: `const` freezes the **reference**, not the object.

```js
const user = { name: "Mohit" };
user.name = "Bansal";   // fine — mutating the object the ref points to
user = {};              // TypeError — cannot reassign the reference itself
```

## 3. Mutation Through Shared References

Because aliases share the underlying object, one mutation ripples everywhere:

```js
const original = { role: "admin" };
const copy = original;         // alias, NOT a copy
copy.role = "user";

console.log(original.role);    // "user" — surprise!
```

Arrays behave identically — they're objects too:

```js
const arr1 = [1, 2, 3];
const arr2 = arr1;
arr2.push(4);
console.log(arr1);             // [1, 2, 3, 4]
```

This is exactly how React/Redux state bugs happen: you "update" state, but you actually mutated the old object, so the framework sees the same reference and skips re-rendering.

## 4. Reassignment vs Mutation — THE Distinction

This is the single most-asked version of this question (it's also preserved as a demo in `code.js`, Section 3):

```js
function rename(obj) {
    obj.name = "Mohit Bansal";     // MUTATION: follows the reference, changes the shared object
}

function rename2(obj) {
    obj = { name: "Mohit Bansal 2" }; // REASSIGNMENT: rebinds the LOCAL variable to a new object
}

const person = { name: "Mohit" };
rename(person);
rename2(person);
console.log(person);               // { name: "Mohit Bansal" }
```

Why does `rename2` "not work"? Because parameters are **local variables**. Assigning to `obj` inside the function only changes what *that local slot* points to. The caller's `person` still points at the old object. You changed the arrow inside the function, not the house.

Rule of thumb:
- **Mutate** (`obj.x = ...`, `arr.push(...)`) → visible to every holder of the reference.
- **Reassign** (`obj = ...`) → visible only inside the current scope.

## 5. Pass-by-Sharing (How JS Passes Arguments)

JS is neither pass-by-value nor pass-by-reference in the C++ sense. It's **pass-by-sharing** (a.k.a. call-by-object-sharing): the function receives a **copy of the reference**.

So:
- You CAN mutate the object the argument points to (caller sees it).
- You CANNOT make the caller's variable point somewhere else.

```js
function reseed(arr) {
    arr.push(99);        // caller sees this...
    arr = [7, 8];        // ...but NOT this
}

const nums = [1];
reseed(nums);
console.log(nums);       // [1, 99]
```

To truly "return something new", return it and let the caller rebind: `nums = makeNewArray()`.

## 6. Shallow vs Deep Copy

Copying matters because you often want a *new* object with the same data — so mutations don't leak back.

### Shallow copy — one level deep

```js
const src = { profile: { city: "Delhi" }, tags: ["a"] };

const c1 = { ...src };
const c2 = Object.assign({}, src);

c1.profile.city = "Mumbai";
console.log(src.profile.city);   // "Mumbai" — nested object still SHARED
```

Spread and `Object.assign` copy the top-level properties **as references**. Top-level primitives are safely duplicated; any nested object is still aliased.

### Deep copy — fully independent

The modern answer:

```js
const deep = structuredClone(src);
deep.tags.push("b");
deep.profile.city = "Goa";
console.log(src.tags);           // ["a"] — untouched
console.log(src.profile.city);   // "Delhi"
```

`structuredClone` handles nesting, arrays, Dates, Maps, Sets, cycles. It does **not** handle functions, DOM nodes, or property descriptors/getters (they're dropped), and it throws on prototypes being preserved? No — it preserves built-ins but loses the custom prototype/class identity (you get plain objects back).

### JSON round-trip and its limits

The classic hack:

```js
const jsonCopy = JSON.parse(JSON.stringify(src));
```

Works for plain JSON-ish data, but silently mangles anything else:
- `undefined`, functions, `Symbol` → **dropped**
- `NaN`, `Infinity` → become `null`
- `Date` → becomes a string
- `Map`/`Set` → become `{}` / lose contents
- cyclic references → **throws TypeError**

Interviewers love asking "what's wrong with `JSON.parse(JSON.stringify())`?" — this list IS the answer.

### Quick comparison table

| Method | Depth | Functions | Dates | Cycles | Notes |
|---|---|---|---|---|---|
| `{...obj}` / `Object.assign` | 1 | kept (refs) | ref | n/a | fastest, idiomatic |
| `JSON.parse(JSON.stringify)` | deep | dropped | string→loses type | throws | JSON-safe data only |
| `structuredClone` | deep | dropped | real Date | handled | modern default |
| recursive manual clone | deep | your call | your call | up to you | great exercise (see exercises.md) |

## 7. Property Descriptors (Brief)

Every property has metadata you can inspect with `Object.getOwnPropertyDescriptor`:

```js
const o = { a: 1 };
Object.getOwnPropertyDescriptor(o, "a");
// { value: 1, writable: true, enumerable: true, configurable: true }
```

- **writable: false** → assignment silently fails (or throws in strict mode).
- **enumerable: false** → hidden from `for...in`, `Object.keys`, spread.
- **configurable: false** → can't delete or reconfigure.
- `Object.freeze(o)` → makes all props non-writable/non-configurable AND blocks new ones (shallow!). `Object.seal` → no add/delete, but writable stays.

Note: freeze is shallow — frozen object containing a mutable array is still mutable inside. Another trap.

## 8. Comparing Objects

`===` on objects compares **references**, never content:

```js
{} === {}                    // false — different objects
[1,2] == [1,2]              // false
const p = { a: 1 };
p === p                      // true — same reference
```

Even `NaN === NaN` is false (primitives have their own fun). For structural equality you need a deep comparison — write your own `isEqual` (see exercises.md) or use a library's `_.isEqual`. Interviewers ask you to implement it precisely because `===` lies about "sameness" of content.

## 9. Common Interview Traps

1. **Reassignment looks like mutation.** `function f(o){ o = {...} }` changes nothing outside. (Section 4.)
2. **Spread is shallow.** `{...state}` then mutating `state.nested` still corrupts the "copy".
3. **`const` ≠ immutable.** `const` locks the binding; object contents stay editable. Combine `const` + `Object.freeze` (still shallow) for real protection.
4. **`JSON.parse(JSON.stringify(x))`** eats functions, `undefined`, Dates, cycles.
5. **Comparing objects with `===` or `.equals`-style thinking** — JS has no built-in deep equality.
6. **Array methods split into mutators vs non-mutators:** `push/splice/sort/reverse` mutate in place; `map/filter/slice/concat/toSorted` return new arrays. `arr.sort()` on shared state is a classic production bug.
7. **Function params are copies of the reference** — you can't swap the caller's variable from inside.
8. **Two identical-looking objects are never equal** without a deep compare.

---

## Where To Go Next

Run the runnable companion: `node js/objects-and-references/code.js` — every `console.log` there has its output annotated, so you can predict-then-check. Then do `exercises.md`: writing your own `deepClone` and `isEqual` is where this model goes from "understood" to "owned".
