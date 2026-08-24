# Arrays and Array Methods

## The Big Idea

An array in JavaScript is not a special primitive type. It is an **object** whose keys
are strings that look like `"0"`, `"1"`, `"2"`… plus a magic `length` property that
stays in sync. Everything else — the methods, the gotchas, the interview traps —
follows from that one fact.

This module covers: what arrays really are, which methods mutate and which don't,
the infamous `sort()` gotcha, the big three (`map` / `filter` / `reduce`),
the rest of the everyday toolkit, chaining, and the patterns interviews actually ask about.

---

## 1. Arrays Are Objects

```js
const arr = [10, 20, 30];
console.log(typeof arr);            // "object"
console.log(Array.isArray(arr));    // true
console.log(arr["1"]);              // 20  <- element access via a STRING key
```

- `typeof []` is `"object"` — there is no `"array"` type in JavaScript.
  This is a classic interview warm-up question.
- The reliable check is **`Array.isArray(value)`**. Use it, not `instanceof Array`
  (which fails across realms/iframes) and definitely not `typeof`.
- Elements live under string keys internally, so `arr[1]` and `arr["1"]` are the same slot.

### Quirks worth knowing

```js
const a = [1, 2, 3];
a.foo = "hi";            // arrays can hold arbitrary object properties
console.log(a.length);   // 3  <- extra props do NOT affect length

const b = [];
b[5] = "x";
console.log(b.length);   // 6  <- assigning past the end extends length (holes!)
```

Because an array is an object, two variables holding "the same array" hold the
**same reference** (see the `objects-and-references` module). Copying an array is a
separate act, and *how* you copy decides whether nested things are shared.

> **Interview line:** "Arrays are objects with integer-like keys and a managed
> `length`; `Array.isArray` is the only trustworthy type check."

---

## 2. Mutating vs Non-Mutating Methods

The single most useful mental model for array methods. When an interviewer says
"does that method mutate?", they mean: does it change the original array?

| Mutates the original | Returns a new array / value |
|----------------------|------------------------------|
| `push`, `pop`, `shift`, `unshift` | `map`, `filter`, `slice`, `concat` |
| `splice` | `flat`, `flatMap`, `reduce` (returns value) |
| `sort`, `reverse` | `toSorted`, `toReversed`, `toSpliced`, `with` |
| `fill`, `copyWithin` | `find*`, `some`, `every`, `includes` (return value/bool) |

```js
const original = [3, 1, 2];
const popped = original.pop();
console.log(original);   // [3, 1]      <- changed!
console.log(popped);     // 2

const base = [3, 1, 2];
const sliced = base.slice(0, 2);
console.log(base);       // [3, 1, 2]   <- untouched
console.log(sliced);     // [3, 1]
```

**Rule of thumb:** if it returns a *new array*, the original is safe; if it returns
a *removed item*, a boolean, or nothing, it probably mutated. (`sort` and `reverse`
are the famous exceptions — they mutate **and** return the same mutated array,
which fools people into thinking they're pure.)

Note `splice` vs `slice`: nearly identical names, opposite behavior. `splice`
performs surgery on the array; `slice` photocopies a section.

### The newer non-mutating copies (ES2023)

Modern engines give immutable counterparts to the old mutators:

```js
const nums = [3, 1, 2];
console.log(nums.toSorted());     // [1, 2, 3]  - nums unchanged
console.log(nums.toReversed());   // [2, 1, 3]  - nums unchanged
console.log(nums.with(0, 99));    // [99, 1, 2] - replace index 0, nums unchanged
console.log(nums.toSpliced(0, 1)); // [1, 2]    - nums unchanged
console.log(nums);                // [3, 1, 2]  <- still intact through all of this
```

Great for React-style state updates where you must not touch previous state.

---

## 3. The `sort()` Gotcha

`sort()` has no default comparator, so it converts every element to a **string**
and sorts lexicographically (by Unicode code units):

```js
console.log([10, 9, 100].sort());          // [10, 100, 9]   <- string order!
console.log(["banana", "Apple"].sort());   // ["Apple", "banana"]  <- case matters
console.log([10, 9, 100].sort((a, b) => a - b)); // [9, 10, 100]  <- numeric ascending
console.log([10, 9, 100].sort((a, b) => b - a)); // [100, 10, 9]  <- descending
```

Three facts interviewers probe:

1. **Default sort is lexicographic**, so numbers sort wrong unless you pass a comparator.
2. **Comparator contract:** return negative if `a` should come first, positive if
   `b` should come first, zero for ties. `(a, b) => a - b` encodes all of that for numbers.
3. **It mutates.** `arr.sort()` reorders `arr` in place. Use `arr.toSorted(cmp)`
   or `[...arr].sort(cmp)` when you need the original preserved.

For strings, `localeCompare` gives human-friendly ordering:
`names.sort((a, b) => a.localeCompare(b))`.

Sorting objects always needs a comparator:

```js
const users = [{ name: "Bo", age: 31 }, { name: "Al", age: 25 }];
users.sort((a, b) => a.age - b.age);
console.log(users[0].name); // "Al"
```

---

## 4. The Big Three: `map`, `filter`, `reduce`

These three cover most data-transformation work. Learn them as a pipeline grammar:
**filter shapes the set → map reshapes each item → reduce collapses to one value.**

### `map` — transform every element, same length out

```js
const nums = [1, 2, 3, 4];
console.log(nums.map(n => n * n));           // [1, 4, 9, 16]
console.log(nums.map((n, i) => `${i}:${n}`)); // ["0:1", "1:2", "2:3", "3:4"]
```

Callback signature: `(element, index, array)`. `map` never skips ahead — output
length equals input length.

### `filter` — keep what passes the test

```js
console.log(nums.filter(n => n % 2 === 0)); // [2, 4]
console.log(nums.filter(n => n > 100));     // []  <- empty, not undefined
```

Returns a subset (possibly empty). Predicate also receives `(element, index, array)`.

### `reduce` — collapse everything into one value (the Swiss Army knife)

Signature: `array.reduce(callback, initialValue)`; callback is
`(accumulator, current, index, array) => newAccumulator`.

Most people know sum. But reduce can build *anything*: objects, maps, arrays,
counts — because the accumulator is just whatever you say it is.

```js
// Sum
const sum = [1, 2, 3, 4].reduce((acc, n) => acc + n, 0);
console.log(sum); // 10

// Max without Math.max spread
const max = [5, 12, 7].reduce((acc, n) => (n > acc ? n : acc));
console.log(max); // 12

// Frequency count -> plain object
const words = ["a", "b", "a", "c", "b", "a"];
const freq = words.reduce((acc, w) => {
  acc[w] = (acc[w] ?? 0) + 1;
  return acc;
}, {});
console.log(freq); // { a: 3, b: 2, c: 1 }

// groupBy -> the single most requested reduce interview exercise
const people = [
  { name: "Bo", dept: "eng" },
  { name: "Al", dept: "sales" },
  { name: "Cy", dept: "eng" },
];
const byDept = people.reduce((acc, p) => {
  (acc[p.dept] ??= []).push(p);
  return acc;
}, {});
console.log(byDept.eng.length); // 2

// Flatten one level of nested arrays
const nested = [[1, 2], [3], [4, 5]];
const flat = nested.reduce((acc, inner) => acc.concat(inner), []);
console.log(flat); // [1, 2, 3, 4, 5]

// Reduce can even implement filter+map in one pass
const evensSquared = [1, 2, 3, 4, 5, 6].reduce(
  (acc, n) => (n % 2 === 0 ? [...acc, n * n] : acc),
  []
);
console.log(evensSquared); // [4, 16, 36]
```

Two habits that make reduce safe:

- **Always pass an initial value** unless you have a reason not to. Without one,
  reduce uses element 0 as the seed and throws on an empty array.
- **Return the accumulator** on every branch. Forgetting `return acc` inside an
  `if` is the #1 reduce bug.

When *not* to use reduce: if a simple loop, `map`, or a `for...of` reads more
clearly, prefer that. Reduce shines when building/collapsing structures, not as
obfuscated syntax.

---

## 5. Finders and Checkers

| Method | Returns | Stops when |
|--------|---------|------------|
| `find(cb)` | first matching **element** or `undefined` | first match |
| `findIndex(cb)` | first matching **index** or `-1` | first match |
| `findLast(cb)` | last matching element | last match (from end) |
| `some(cb)` | `true` if **at least one** passes | first match |
| `every(cb)` | `true` if **all** pass | first failure |
| `includes(x)` | `true` if value present (uses SameValueZero) | found |

```js
const nums = [1, 3, 5, 8, 9];
console.log(nums.find(n => n % 2 === 0));  // 8
console.log(nums.findIndex(n => n > 5));   // 3
console.log(nums.some(n => n > 8));        // true
console.log(nums.every(n => n > 0));       // true
console.log(nums.includes(5));             // true
console.log([NaN].includes(NaN));          // true  <- unlike indexOf!
console.log([NaN].indexOf(NaN));           // -1
```

Interview nugget: `includes` finds `NaN`, `indexOf` cannot — they use different
equality algorithms. Also, `find` returning `undefined` is ambiguous ("not found"
vs "found `undefined`"), which is why `findIndex` exists.

---

## 6. `flat` and `flatMap`

```js
console.log([1, [2, [3, [4]]]].flat());        // [1, 2, [3, [4]]]  - depth 1 default
console.log([1, [2, [3, [4]]]].flat(2));       // [1, 2, 3, [4]]
console.log([1, [2, [3, [4]]]].flat(Infinity)); // [1, 2, 3, 4]  - fully flat

// flatMap = map then flatten depth 1; great for expand-one-to-many
const sentences = ["hello world", "hi there"];
console.log(sentences.flatMap(s => s.split(" ")));
// ["hello", "world", "hi", "there"]
```

`flatMap` beats `.map(...).flat()` because it walks the array once.

---

## 7. Chaining

Because `map`, `filter`, `flatMap`, `slice`, `concat`, and friends return arrays,
you compose pipelines left to right:

```js
const orders = [
  { id: 1, total: 250, status: "paid" },
  { id: 2, total: 80,  status: "open" },
  { id: 3, total: 500, status: "paid" },
];

const paidTotalsOver200 = orders
  .filter(o => o.status === "paid")
  .filter(o => o.total > 200)
  .map(o => o.total)
  .reduce((sum, t) => sum + t, 0);

console.log(paidTotalsOver200); // 750
```

Chaining etiquette:

- **Filter early, map late** — shrink the dataset before transforming it.
- Each link copies (non-mutating), so chains are safe but not free; for huge
  arrays consider one `reduce` pass.
- A chain ends whenever a link returns something that isn't an array
  (e.g., final `reduce`, `some`, `find`) — that's your result, not a chain failure.

---

## 8. Sparse Arrays (brief)

Holes are positions that were never assigned. They behave inconsistently across
methods, which is exactly why you avoid creating them:

```js
const sparse = [1, , 3];              // hole at index 1
console.log(sparse.length);           // 3
console.log(sparse[1]);               // undefined
console.log(1 in sparse);             // false  <- the slot doesn't exist
console.log(sparse.map(n => n * 2));  // [2, <1 empty item>, 6]  <- holes preserved
console.log(sparse.join("-"));        // "1--3"  <- hole becomes empty string
```

Rules of thumb: `delete arr[i]` creates a hole (use `splice` instead);
iteration methods generally skip-or-preserve holes unpredictably enough that
you should treat holes as a bug source. `Array.from` and `new Array(n).fill()`
give dense arrays when you need fixed-size scaffolding.

---

## 9. `Array.from` and `Array.of`

```js
// From any iterable or array-like
console.log(Array.from("abc"));                 // ["a", "b", "c"]
console.log(Array.from({ length: 3 }, (_, i) => i * 10)); // [0, 10, 20]
console.log(Array.from(new Set([1, 1, 2])));    // [1, 2]  - dedupe trick

// Array.of always makes an array of its arguments (unlike new Array)
console.log(new Array(3));   // [<3 empty items>]  <- trap: length 3, no elements
console.log(Array.of(3));    // [3]
console.log(Array.of(1, 2)); // [1, 2]
```

`Array.from(mapFn)` doubles as "build an array programmatically" — the go-to for
ranges and test fixtures. Remember `[...new Set(arr)]` as the idiomatic one-line
dedupe.

---

## 10. Common Interview Patterns

- **Dedupe:** `[...new Set(arr)]` — know why it works (Set preserves insertion order).
- **Frequency counting:** `reduce` into an object (or `Map`).
- **Grouping:** the `groupBy` reduce shown above; be ready to write it cold.
- **Chunking:** slice inside a loop — `arr.slice(i, i + size)`.
- **Intersection/difference:** combine `filter` + `includes` (or `Set.has` for O(n)).
- **Second-largest number:** sort copy vs one-pass reduce tracking two values.
- **Flatten arbitrarily deep:** recursion, or `flat(Infinity)`.
- **Rotate an array:** `slice` + `concat` — e.g. rotate left by k:
  `arr.slice(k).concat(arr.slice(0, k))`.

If you internalize mutation rules + sort's comparator + reduce-as-builder, you can
derive almost every array question instead of memorizing answers.

---

## Quick Recap

- Arrays are objects; check with `Array.isArray`.
- Know your mutators (`push/pop/splice/sort/reverse/fill`) and prefer their
  non-mutating twins (`slice/concat/toSorted/toReversed/toSpliced/with`).
- `sort()` defaults to string comparison and mutates — always pass a comparator.
- `map` transforms, `filter` selects, `reduce` builds anything.
- `find/some/every/includes` answer questions; `flat/flatMap` handle nesting;
  `Array.from/of` construct arrays safely.
