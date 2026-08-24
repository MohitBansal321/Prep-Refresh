# Primitives and Types — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | JavaScript language fundamentals (type system). |
| **Core Idea** | Every value is one of **7 immutable primitives** or an **object**. Primitives are compared & copied **by value**; objects by **reference**. |
| **The 7 Primitives** | `string` · `number` · `bigint` · `boolean` · `undefined` · `null` · `symbol`. No separate integer type — `number` is a 64-bit float. |
| **typeof Quirks** | `typeof null` → `"object"` (historic bug) · functions → `"function"` · arrays → `"object"` (use `Array.isArray`) · `NaN` → `"number"` · undeclared var → `"undefined"` without throwing. |
| **Immutability** | Primitives can never be changed in place — every "mutation" returns a NEW value (`"a".toUpperCase()` doesn't touch the original; `str[0] = "x"` silently fails). |
| **Copying (value copy)** | Primitives: assignment/arguments get independent copies. Objects: copies share the reference — mutating through one alias affects all. `{ ...obj }` is shallow only. |
| **Gotchas** | `NaN !== NaN` (test with `Number.isNaN`) · `0.1 + 0.2 !== 0.3` (epsilon / integer cents) · integers unsafe past `Number.MAX_SAFE_INTEGER` (2^53−1) · auto-boxing makes `new String("a") === "a"` false · `1n + 1` throws TypeError · properties set on primitives vanish. |
| **Use When** | `bigint`: IDs/crypto beyond 2^53 · `symbol`: collision-proof keys, iterables via `Symbol.iterator`, registry via `Symbol.for` · `== null` idiom: the one sanctioned loose-equality check. |
| **Related Topics** | Type coercion (ToNumber/ToPrimitive) · truthiness/falsy list · shallow vs deep copy (`structuredClone`) · equality vs identity · wrapper objects (`String`/`Number`/`Boolean`). |

### Skeleton
```js
typeof null;              // "object"   <- bug, check null with === null
typeof function(){};      // "function"
"abc".length;             // 3          <- auto-boxing: temp String wrapper, then discarded
0.1 + 0.2 === 0.3;        // false      <- IEEE-754 doubles; use Number.EPSILON tolerance
Number.isNaN(NaN);        // true       <- NaN !== NaN, never test with === NaN
9007199254740993n;        // bigint: exact past 2^53-1; mixing with number throws
Symbol("id") === Symbol("id"); // false <- every symbol unique; Symbol.for("k") shares
```

### Remember In One Sentence
> **Primitives are the 7 immutable leaf values copied by value while everything else is a mutable object shared by reference — so know your `typeof` quirks, box-free method calls, and where coercion and float math betray you.**

### Two Facts People Get Wrong
- `typeof null` returns **`"object"`**, not `"null"` — a preserved 1995 bug. Test null with `x === null`; test arrays with `Array.isArray`.
- `"abc".length` works because of **auto-boxing**, but `new String("abc") === "abc"` is **false** (boxed object vs primitive), and properties you add to a primitive silently disappear.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README/code.js. If you miss one, that section is all you need to re-study.

1. List all 7 primitive types from memory. Why is there no integer type among them?
2. What does `typeof` return for: `null`, an array, a function, `NaN`, and an *undeclared* variable? Which two answers surprise people?
3. Explain what happens internally when you call `"hello".toUpperCase()` — name the mechanism, what gets created and discarded, and why the original string survives.
4. Predict `let pet = "cat"; pet.color = "black"; console.log(pet.color)` and explain exactly where the property went.
5. Is JavaScript pass-by-value or pass-by-reference? Give the precise answer covering both primitives and objects, including what a function can and cannot do to its arguments.
6. Name three differences between `==` and `===`, plus the single loose-equality idiom considered acceptable in modern code.
7. Why does `value === NaN` never work, how do `isNaN` and `Number.isNaN` differ, and which should you use?
8. Explain why `0.1 + 0.2 !== 0.3`, give two ways to compare floats safely, and state the exact integer ceiling for `number` — along with the type that fixes it.
