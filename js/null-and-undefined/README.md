# null and undefined — JavaScript's Two Kinds of "Nothing"

## Intent

Understand exactly **where `undefined` comes from, what `null` means, how JavaScript
distinguishes (and fails to distinguish) them** — and how to read values safely with
`?.`, fall back correctly with `??`, and stop `||` from eating your legitimate falsy
values.

This is one of the most-asked interview topics at every level, because it looks trivial
and hides real depth.

## Real Life Analogy

Imagine a hotel receptionist holding your room key envelope.

- The envelope is **`undefined`**: nobody has put anything in it yet. The *absence* is
  just "not filled in." No decision was made; the slot is simply empty.
- The envelope contains a note saying **"no key"** (`null`): someone deliberately opened
  it and wrote that. A human made a choice: "this guest definitely has no room."

Both mean "you get no key today" — but they carry different information. `undefined` is
*the system shrugging*; `null` is *a person answering "none."* That single distinction
explains almost everything below.

## Part 1 — What `undefined` Means

`undefined` means **"no value has been assigned."** You rarely write it yourself;
JavaScript produces it automatically in four situations:

### 1. Variable declared but not initialized

```js
let name;
console.log(name); // undefined
```

The binding exists, but no value occupies it. (`const` never does this — you must
initialize it, or you get a SyntaxError.)

### 2. Function with no return value

```js
function greet() {
    console.log("Hello");
}
console.log(greet()); // undefined
```

Every function returns something. If there is no `return`, the return value is
`undefined`. This bites people who call a function expecting data back:

```js
const result = saveToDatabase(record); // saveToDatabase returns nothing
console.log(result.id); // TypeError!
```

### 3. Accessing a non-existent object property

```js
const obj = { name: "Mohit" };
console.log(obj.age); // undefined
```

Note: JavaScript does not error on *reading* missing properties (unlike most languages).
It silently hands back `undefined` and lets the crash happen one line later when you try
to use it. That deferred failure is the source of countless production bugs.

### 4. Function parameters not provided

```js
function add(a, b) {
    return a + b;
}
console.log(add(1)); // NaN  — b is undefined, 1 + undefined = NaN
```

Missing arguments become `undefined`, not an error and not `0`.

### 5. (Bonus) The `void` operator

`void <anything>` always evaluates to `undefined`. Historically used to obtain a
guaranteed `undefined` even before ES5 made `undefined` non-writable:

```js
console.log(void 0 === undefined); // true
```

You still see `void 0` in minified bundles for this reason.

## Part 2 — What `null` Means

`null` means **"intentional absence of a value."** Unlike `undefined`, somebody writes
it — usually to clear a reference or to answer "we checked, there is none":

```js
let user = { name: "Mohit" };
user = null; // deliberate: the user reference is emptied

let profile = {
    nickname: undefined, // never chosen by the user
    spouse: null,        // looked up in the DB, confirmed none exists
};
```

A widely-used convention worth internalizing (and mentioning in interviews):

| Value | Meaning | Who produced it |
|-------|---------|-----------------|
| `undefined` | "not set yet" | the language |
| `null`      | "deliberately empty / none found" | you (or your API) |

Functions like `document.querySelector()` return `null` — not `undefined` — precisely
because they *searched and found nothing*, which is an answer, not an omission.

## Part 3 — The typeof Quirk

```js
typeof undefined; // "undefined"
typeof null;      // "object"   <- the famous bug
```

`typeof null === "object"` is a mistake from JavaScript's first implementation in 1995
(null was represented with a type tag shared with objects). It cannot be fixed because
millions of sites depend on the current behavior.

Practical consequences:

1. **Never use `typeof x === "object"` alone as an object check.**
   ```js
   function describe(v) {
       if (typeof v === "object") return "object-ish"; // WRONG for null
   }
   describe(null); // "object-ish" — classic trap
   ```
2. **Check for null explicitly first:** `if (v !== null && typeof v === "object")`.
3. To distinguish them reliably, use strict equality:
   `x === null`, `x === undefined`, or `x == null` (see next section).

## Part 4 — `==` vs `===` Between Them

```js
null == undefined;   // true   — loose equality treats them as equivalent emptiness
null === undefined;  // false  — strict equality sees different types
null != undefined;   // false
null !== undefined;  // true
```

Loose equality has exactly **one** special rule here: `null` equals `undefined` and
nothing else:

```js
null == 0;       // false
null == "";      // false
null == false;   // false
undefined == 0;  // false
```

This yields a beloved idiom — the compact "is empty" guard:

```js
if (value == null) { /* true ONLY for null or undefined */ }
```

It works only because of that single special case; `0` and `""` do not pass.
Interviewers like asking *why* `x == null` is safe when `==` is otherwise banned.

## Part 5 — Falsy Values: the Complete List

Exactly six values are falsy. Everything else is truthy:

```js
false, 0, "" , null, undefined, NaN
```

(Strictly, `-0` and `0n` count within those families.) The interview-relevant part is
what is **truthy despite looking empty**:

```js
Boolean([]);      // true  — empty array!
Boolean({});      // true  — empty object!
Boolean("0");     // true  — non-empty string
Boolean("false"); // true  — non-empty string!
```

Memorize the six-item list; "empty-looking things" like `[]` and `{}` are NOT on it.

## Part 6 — Optional Chaining (`?.`)

Reading deep into data is where `undefined` hurts most:

```js
order.user.name;        // TypeError if order.user is undefined
order.user?.name;       // undefined instead of throwing
```

`?.` short-circuits the whole chain to `undefined` the moment it hits `null` or
`undefined`:

```js
const order = { id: 7, customer: { address: { city: "Pune" } } };

order.customer?.address?.city; // "Pune"
order.user?.name;              // undefined — no throw
order.user?.address?.city;     // undefined — chain stops early
```

It also guards method calls and index access:

```js
api.fetch?.();   // calls only if api.fetch exists
list?.[0];       // undefined when list is null/undefined
```

Two subtleties:

- `?.` checks **only** nullish values. If `customer.address` exists but `city` is
  `"", 0`, etc., you still get that value.
- If a property exists but holds `null` (e.g. `weird.tag = null`), then
  `weird.tag?.toUpperCase()` short-circuits to `undefined` — the null stops the call.

## Part 7 — Nullish Coalescing (`??`) vs Logical OR (`||`)

The single most valuable distinction in this module:

- `a || b` → falls back to `b` when `a` is **falsy** (any of the six).
- `a ?? b` → falls back to `b` when `a` is **nullish** (`null` or `undefined` only).

```js
const volume = 0;

volume || 50;  // 50  <- BUG! 0 is a valid volume, || threw it away
volume ?? 50;  // 0   <- correct: 0 is not nullish
```

More contrasts:

```js
"" || "default";      // "default"
"" ?? "default";      // ""
false || true;        // true
false ?? true;        // false
null ?? "fallback";   // "fallback"
0 ?? "fallback";      // 0
```

`??` picks the **first non-nullish** value in a chain:

```js
null ?? undefined ?? 3 ?? 10; // 3
```

Syntax rule: you may not mix `??` directly with `||`/`&&` — parenthesize:

```js
// null || undefined ?? "x"          // SyntaxError
(null || undefined) ?? "x";         // "x"
```

**Rule of thumb:** defaulting user/config values? Use `??`. Only reach for `||` when
collapsing *all* falsy values is genuinely what you mean.

## Part 8 — Default Parameters Trigger on `undefined` ONLY

Default parameter values fire when the argument is **missing or explicitly
`undefined`** — never for `null`, `0`, `""`, or `NaN`:

```js
function connect(host = "localhost") {
    return host;
}

connect();           // "localhost"
connect(undefined);  // "localhost"  — explicit undefined triggers the default
connect(null);       // null   <- default does NOT fire!
connect("");         // ""
```

This surprises everyone eventually: passing `null` "to use the default" silently hands
`null` downstream instead. If a caller might pass `null` but you want the default,
normalize at the boundary:

```js
host = host ?? "localhost"; // catches both null and undefined
```

Inside the function body, defaults written as assignments behave like `??` — that is
exactly the mental model.

## Part 9 — Common Bugs From Each

**From `undefined`:**

1. *"Cannot read properties of undefined"* — reading a property off something that
   turned out missing. Fix: `?.` plus a `??` fallback:
   ```js
   const city = orderLike?.customer?.address?.city ?? "unknown";
   ```
2. Expecting a return value from a function that doesn't `return`.
3. `add(1)` producing `NaN` because the second arg became `undefined`.

**From `null`:**

4. `||` discarding valid `0` / `""` values — use `??`.
5. Passing `null` hoping a default parameter kicks in — it doesn't.
6. `typeof null === "object"` fooling object checks — test `!== null` first.
7. Confusing "query returned `null`" (checked, none) with "field is `undefined`"
   (never set) — different diagnoses need different fixes.

## Summary

- `undefined` = the system says "nothing assigned yet" (4 automatic sources + `void`).
- `null` = you say "deliberately empty."
- `==` lumps them together; `===` separates them; `x == null` matches exactly those two.
- Falsy list: `false, 0, "", null, undefined, NaN` — `[]` and `{}` are truthy.
- `?.` prevents deep-read throws; `??` falls back only on nullish; `||` falls back on
  all falsy (often wrongly).
- Default parameters fire on `undefined` only — never `null`.
