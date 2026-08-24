# Primitives and Types

## What Are Primitives?

In JavaScript, a **primitive** (primitive value, primitive data type) is data that is **not an object** and has **no methods or properties of its own**. Primitive values are **immutable** — they cannot be changed after creation.

This is one of those foundational questions interviewers use as a warm-up — but it has teeth. Follow-ups like *"why does `typeof null` return `"object"`?"*, *"how can `"abc".length` work if primitives have no properties?"*, or *"what's the difference between copying a string and copying an object?"* separate candidates who memorized a list from candidates who actually understand the type system.

Everything in JavaScript is either a **primitive** (7 types) or an **object** (everything else: plain objects, arrays, functions, dates, maps, sets...). That's the whole taxonomy. Master these 7 and `typeof`, and you've mastered half of the "JavaScript fundamentals" interview section.

```js
// A primitive is a leaf value — it contains nothing else.
const str = "hello";    // string
const num = 10;         // number
const bigInt = 123n;    // bigint
const bool = true;      // boolean
const undef = undefined;// undefined
const nul = null;       // null
const sym = Symbol("id"); // symbol

console.log(typeof nul); // "object"  <-- remember this quirk (below)
```

> **Term: Immutability.** An immutable value can never be modified in place. Any operation that seems to "change" a primitive (like `"hello".toUpperCase()`) actually **returns a brand-new value**, leaving the original untouched. Contrast with arrays/objects, which *are* mutable in place.

---

## The 7 Primitive Types

| Type | Example | `typeof` returns | Notes |
|------|---------|------------------|-------|
| `string` | `"hello"` | `"string"` | Immutable sequence of UTF-16 code units |
| `number` | `10`, `0.5`, `NaN`, `Infinity` | `"number"` | IEEE-754 double — there is **no separate integer type** |
| `bigint` | `123n` | `"bigint"` | Arbitrary-precision integers (ES2020) |
| `boolean` | `true` / `false` | `"boolean"` | Only two values |
| `undefined` | `undefined` | `"undefined"` | Declared-but-unassigned; also `typeof` on undeclared vars |
| `null` | `null` | **`"object"`** ⚠️ | Intentional absence of a value; the historic `typeof` bug |
| `symbol` | `Symbol("id")` | `"symbol"` | Unique, immutable identifiers (ES2015) |

Two things to internalize:

1. **There is no integer type.** `10` and `10.5` are both `number` (a 64-bit float). Integer behavior only appears via operators (`| 0`, `Math.trunc`) or by using `bigint`.
2. **`undefined` vs `null`:** `undefined` means *"this was never assigned"* (the engine's default); `null` means *"deliberately set to nothing"* (the programmer's choice). Interviewers love this distinction.

---

## typeof Quirks

`typeof x` returns a **string** naming the type. It's mostly reliable, with famous exceptions:

```js
console.log(typeof null);            // "object"  <-- 30-year-old bug, kept for backwards compat
console.log(typeof function(){});    // "function" <- special-cased, though functions ARE objects
console.log(typeof []);              // "object"   <- arrays are objects; use Array.isArray([])
console.log(typeof NaN);             // "number"   <- NaN IS a number, just an invalid one
console.log(typeof undeclaredVar);   // "undefined" <- safe! does NOT throw on undeclared vars
console.log(typeof class {});        // "function"
```

Why `typeof null === "object"`? In the original JS engine (1995), values were tagged with type bits; `null`'s tag was all-zeros, which collided with the object tag. The bug was acknowledged decades ago but fixing it would break millions of websites.

The practical consequences:

- **Testing for null:** never use `typeof`. Use `x === null`.
- **Testing for arrays:** never use `typeof`. Use `Array.isArray(x)`.
- **Safe existence checks:** because `typeof` doesn't throw on undeclared variables, `typeof foo !== "undefined"` is the classic guard for possibly-nonexistent globals.

---

## Primitive Immutability

Once created, a primitive **cannot be altered**. Every apparent mutation creates a new value:

```js
let name = "Mohit";
name.toLowerCase();      // returns "mohit" — but nothing receives it!
console.log(name);       // "Mohit"  — original untouched

let str2 = "hello";
str2[0] = "m";           // silently ignored (sloppy mode)
console.log(str2);       // "hello"

str2 = 'M' + str2.slice(1); // the ONLY way: build a NEW string
console.log(str2);          // "Mello"
```

This explains why string methods always **return** something (`slice`, `toUpperCase`, `trim`) rather than modifying in place — unlike `array.sort()` or `array.push()`, which mutate.

Immutability has real performance implications: building a huge string by repeated concatenation creates many intermediate strings (engines optimize this, but the mental model matters). It also makes primitives safe to share: passing `"hello"` around can never let someone corrupt your copy.

---

## Pass-By-Value vs Pass-By-Reference

Here's the distinction that causes the most production bugs:

- **Primitives** are **copied by value**. Assignment and function arguments get an independent copy.
- **Objects** (including arrays, functions, dates) are **copied by reference**. Both variables point at the *same* object in memory.

```js
// Primitives: independent copies
let p1 = 42;
let p2 = p1;
p2 = 100;
console.log(p1, p2); // 42 100  — original unaffected

// Objects: shared reference
let o1 = { count: 1 };
let o2 = o1;
o2.count = 99;
console.log(o1.count); // 99  — SAME object!

// Same story inside functions:
function tryToChangePrimitive(x) { x = 999; }     // rebinds local copy
function changeObject(objArg) { objArg.count = -1; } // mutates the shared object
let prim = 7, obj = { count: 7 };
tryToChangePrimitive(prim);
changeObject(obj);
console.log(prim, obj.count); // 7 -1
```

Comparison follows the same rule: primitives compare **by value**, objects compare **by reference**:

```js
"a" === "a";           // true  — same value
({}) === ({});         // false — different objects, even if identical-looking
obj1 == obj2;          // false for two separately-created objects with same content
```

Interview phrasing worth practicing: JavaScript is *always pass-by-value* — but when the value happens to be a reference to an object, what gets copied is that reference. So you can mutate the object a parameter points to, but you can't make the caller's variable point somewhere else.

> **Term: Shallow vs deep copy.** `{ ...obj }` and `Object.assign()` copy top-level properties only; nested objects are still shared references. `structuredClone(obj)` gives a true deep copy.

---

## Wrapper Objects (Auto-Boxing)

Wait — if primitives have no methods, how does this work?

```js
console.log("abc".length);        // 3
console.log("abc".toUpperCase()); // "ABC"
console.log((5).toFixed(2));      // "5.00"
```

Answer: **auto-boxing**. When you access a property or method on a primitive, JavaScript temporarily wraps it in a wrapper object (`String`, `Number`, `Boolean`), reads/calls what you asked for, then immediately throws the wrapper away:

```js
// Internally, "hello".toUpperCase() is roughly:
let temp = new String("hello");
temp.toUpperCase();  // method found on String.prototype via the wrapper
temp = null;         // wrapper discarded — the primitive itself was never touched
```

Consequences interviewers probe:

```js
// The wrapper constructors exist — never call them:
const boxed = new String("abc");
console.log(typeof boxed);     // "object"  — not "string"!
console.log(boxed === "abc");  // false — object vs primitive
console.log(boxed == "abc");   // true — loose == coerces (another reason to avoid ==)

// Properties you add vanish instantly with the throwaway wrapper:
let pet = "cat";
pet.color = "black";
console.log(pet.color);        // undefined — the box was garbage-collected

// null and undefined have NO wrappers:
null.foo;  // TypeError: Cannot read properties of null
undefined.foo; // TypeError
```

Rule of thumb: treat `new String()`, `new Number()`, `new Boolean()` as bugs. If you ever need to unwrap one (legacy code), use `.valueOf()`.

---

## == vs === (Coercion Basics)

- **`===` strict equality:** compares type AND value. No conversion. Different types → automatically `false`.
- **`==` loose equality:** coerces operands to a common type first, then compares.

```js
1 == "1";           // true  — "1" becomes 1
1 === "1";          // false
null == undefined;  // true  — special case: loosely equal to each other only
null === undefined; // false
"" == 0;            // true  — "" becomes 0
false == "";        // true  — both become 0
NaN == NaN;         // false — see next section
```

The coercion rules behind `==` (ToNumber/ToPrimitive) are notoriously unintuitive (`[] == ![]` is `true`!). The professional convention: **always use `===`**, except one deliberate idiom — `x == null` catches both `null` and `undefined` in a single check.

Related concept: **truthiness**. In boolean contexts (`if (x)`), values coerce to `true`/`false`. Falsy list — memorize it exactly: `false`, `0`, `-0`, `0n`, `""`, `null`, `undefined`, `NaN`. Everything else is truthy, including surprises:

```js
Boolean("0"); // true  — non-empty string!
Boolean([]);  // true  — empty array is an OBJECT
Boolean({});  // true
```

---

## NaN Behavior

`NaN` ("Not a Number") is the number type's error sentinel — the result of failed numeric conversions or math:

```js
console.log(typeof NaN);       // "number"  — yes, it's technically a number
console.log("hello" * 2);      // NaN
console.log(Math.sqrt(-1));    // NaN
console.log(parseInt("abc"));  // NaN
console.log(undefined + 1);    // NaN
```

Its defining property: **NaN is not equal to anything, including itself.**

```js
NaN === NaN; // false
NaN == NaN;  // false
```

Which means the naive check fails silently:

```js
if (value === NaN) { /* NEVER runs */ }
```

Use instead:

```js
Number.isNaN(value); // ES6 — true ONLY if value is literally NaN. No coercion.
isNaN(value);        // older — COERCES first, so isNaN("abc") is true too
```

That coercion difference matters: `isNaN("abc")` returns `true` because `Number("abc")` is `NaN`; `Number.isNaN("abc")` returns `false` because `"abc"` isn't the value `NaN`. Prefer `Number.isNaN`.

Also know: any arithmetic with `NaN` propagates `NaN`, and `NaN < 1`, `NaN > 1` are both `false`.

---

## Number Gotchas

JS numbers are **IEEE-754 double-precision floats**, which produces two classic gotchas.

### Floating-point imprecision

Binary floats can't represent most decimals exactly:

```js
console.log(0.1 + 0.2);          // 0.30000000000000004
console.log(0.1 + 0.2 === 0.3);  // false — THE classic interview question
```

Fixes:

```js
// Compare with a tolerance (epsilon):
Math.abs(0.1 + 0.2 - 0.3) < Number.EPSILON; // true
// Or do money math in integer cents:
(10 + 20) / 100; // 0.3 — exact
```

Never store money as decimal floats — this is a systems-design point as much as a language one.

### Safe integer limits

Doubles exactly represent integers only up to **2^53 − 1** (`Number.MAX_SAFE_INTEGER` = 9007199254740991):

```js
console.log(Number.MAX_SAFE_INTEGER + 1 === Number.MAX_SAFE_INTEGER + 2); // true — precision lost!
Number.isSafeInteger(Number.MAX_SAFE_INTEGER);    // true
Number.isSafeInteger(Number.MAX_SAFE_INTEGER + 1);// false
```

This bites in real life with JSON IDs from databases/snowflake generators (64-bit ints) — they arrive as strings precisely because of this limit.

Parsing differences worth knowing: `parseInt("42px")` → `42` (stops at junk), `Number("42px")` → `NaN` (all-or-nothing).

---

## BigInt

`bigint` exists precisely to fix the safe-integer ceiling: integers of **arbitrary size, always exact**.

```js
const huge = 1234567890123456789012345678901234567890n;
console.log(huge + 1n);              // exact, no precision loss
console.log(typeof 1n);              // "bigint"

9007199254740993n === 9007199254740993n; // true — bigint literals are exact...
9007199254740993 === 9007199254740992;   // true — ...but as numbers they BOTH round to ...992!
```

Rules and restrictions:

```js
1n + 1;    // TypeError — mixing bigint and number throws (no silent coercion)
1n == 1;   // true  — loose == allows cross-type comparison
1n === 1;  // false — strict sees different types
7n / 2n;   // 3n    — division truncates toward zero; bigint is integer-only
Math.round(10n); // TypeError — Math.* doesn't accept bigint
BigInt(0.5);     // RangeError — no fractional values
```

When to reach for it: cryptographic keys, 64-bit IDs, precise counters beyond 2^53. When NOT to: everyday counts (numbers are faster), money with decimals (bigint can't hold cents-fractions... though cents-as-bigint works).

---

## Symbol Use Cases

A `symbol` is a **guaranteed-unique** identifier. Even identical descriptions produce different symbols:

```js
const s1 = Symbol("id");
const s2 = Symbol("id");
console.log(s1 === s2);     // false
console.log(s1.description); // "id" — description is just a label, not identity
```

Practical uses:

**1. Collision-proof object keys.** Symbol keys don't appear in `Object.keys`, `for...in`, or `JSON.stringify` — semi-private fields that can't clash with string keys:

```js
const user = { name: "Ada" };
user[Symbol("id")] = 42;
Object.keys(user);                    // ["name"] — symbols invisible
JSON.stringify(user);                 // '{"name":"Ada"}'
Object.getOwnPropertySymbols(user);   // still reachable explicitly
```

**2. Well-known symbols** — customization hooks for language behavior. The most cited: `[Symbol.iterator]` makes an object work with `for...of` and spread:

```js
const range = {
  from: 1, to: 3,
  [Symbol.iterator]() {
    let current = this.from;
    const last = this.to;
    return { next: () => ({ done: current > last, value: current++ }) };
  }
};
console.log([...range]); // [1, 2, 3]
```

Others: `Symbol.toPrimitive` (control type conversion), `Symbol.hasInstance` (customize `instanceof`). Libraries like React used symbol-based markers (`$$typeof`) to tag element types safely.

**3. Global registry** — when modules need to share the same symbol across realms/files:

```js
Symbol.for("app.token") === Symbol.for("app.token"); // true — same key, same symbol
Symbol.keyFor(Symbol.for("app.token"));               // "app.token"
```

One more edge case: symbols are primitives, but `new Symbol()` **throws** — there's deliberately no wrapper constructor.

---

## Common Mistakes

- **Checking null with `typeof`.** Returns `"object"` for `null`. Use `x === null`.
- **Comparing with `=== NaN`.** Always false. Use `Number.isNaN(x)`.
- **Expecting `0.1 + 0.2 === 0.3`.** False. Compare with epsilon or use integer cents.
- **Calling `new String("x")` / `new Number(x)`.** Creates boxed objects: wrong `typeof`, broken `===`. Just use literals.
- **Assuming setting a property on a primitive works.** `pet.color = "black"` succeeds silently, then vanishes — reading it back gives `undefined`.
- **Confusing shallow spread with deep copy.** `{ ...obj }` shares nested references.
- **Mixing `bigint` and `number` in arithmetic.** Throws TypeError; convert explicitly.
- **Using `==` casually.** Coercion surprises (`false == ""`, `null == undefined`) will eventually cost you. Reserve it for the `x == null` idiom.

## Interview Discussion

Common follow-up chains and strong answers:

- *"How can primitives have methods if they have none?"* Auto-boxing — the engine wraps them in `String`/`Number`/`Boolean` momentarily, calls the method on the wrapper's prototype, and discards it. The primitive is never mutated; methods return new primitives.
- *"Is JavaScript pass-by-value or pass-by-reference?"* Pass-by-value always — but the value may be a reference (for objects). You can mutate through a reference; you can't rebind the caller's variable.
- *"Why keep the `typeof null` bug?"* Backwards compatibility — fixing it breaks the web. Know the workaround (`x === null`).
- *"How would you compare two values for equality deeply?"* Recursively: primitives compare with `===` (plus `Number.isNaN` handling), objects compare key sets recursively, handle arrays/null/dates. This is a standard whiteboard exercise — see `exercises.md`.
- *"When would you actually use a Symbol?"* Non-colliding object keys, library metadata tags, well-known symbols like `Symbol.iterator` to implement iterables.
- *"Why do JSON APIs send big IDs as strings?"* Because JSON numbers parse into doubles, losing precision past 2^53 − 1.

## Summary

- Exactly **7 primitives**: string, number, bigint, boolean, undefined, null, symbol. Everything else is an object.
- Primitives are **immutable** and compared/copied **by value**; objects by **reference**.
- `typeof` quirks to recite cold: `typeof null` → `"object"`, functions → `"function"`, arrays → `"object"` (use `Array.isArray`), undeclared vars → `"undefined"` without throwing.
- Methods on primitives work via **auto-boxing**; wrapper constructors themselves are a trap.
- `===` never coerces; `==` does — default to `===`.
- `NaN` is a `number`, unequal to everything; test with `Number.isNaN`.
- Numbers are IEEE-754 doubles: expect `0.1 + 0.2 !== 0.3` and precision loss past 2^53 − 1; `bigint` fixes integer precision.

## Key Takeaways

1. 7 primitives, everything else is an object — say them from memory: string, number, bigint, boolean, undefined, null, symbol.
2. Immutability means every "change" to a primitive returns a new value; the original is untouchable.
3. Primitives copy/compare by value, objects by reference — this drives half of all JS equality bugs.
4. `typeof null === "object"` is a preserved historical bug; check null with `=== null`.
5. Auto-boxing explains `"abc".length`; `new String("abc") === "abc"` is `false`.
6. Default to `===`; the one sanctioned `==` usage is `x == null` for null-or-undefined.
7. `NaN !== NaN` — detect it with `Number.isNaN`, never `=== NaN` or bare `isNaN` (which coerces).
8. `0.1 + 0.2 !== 0.3` and integers die past `Number.MAX_SAFE_INTEGER`; use epsilon comparison or `bigint`.

---

## Further Reading

- MDN — JavaScript data types and data structures — https://developer.mozilla.org/en-US/docs/Web/JavaScript/Data_structures
- MDN — Equality comparisons and sameness — https://developer.mozilla.org/en-US/docs/Web/JavaScript/Equality_comparisons_and_sameness
- MDN — `Symbol` (incl. well-known symbols) — https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Symbol
- "What every computer scientist should know about floating-point arithmetic" — David Goldberg (background for the 0.1 + 0.2 problem)
- javascript.info — Primitives & objects, Type conversions — https://javascript.info
