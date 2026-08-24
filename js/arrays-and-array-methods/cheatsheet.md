# Arrays and Array Methods — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core JavaScript data structure + everyday method toolkit. |
| **Arrays Are Objects** | `typeof []` is `"object"`; elements live under string keys (`arr[1]` ≡ `arr["1"]`); extra props don't affect `length`. Only trust `Array.isArray()` for type checks (not `instanceof`). |
| **Mutating Methods** | `push/pop/shift/unshift`, `splice`, `sort`, `reverse`, `fill`, `copyWithin` — they change the original array in place. |
| **Non-Mutating Methods** | `map`, `filter`, `slice`, `concat`, `flat`, `flatMap` return new arrays/values; originals untouched. |
| **Newer Non-Mutating Copies** | ES2023 twins: `toSorted()`, `toReversed()`, `toSpliced()`, `with(index, value)` — immutable versions of the old mutators. |
| **sort Gotcha** | Default comparator converts to **strings** → lexicographic order (`[10,9,100].sort()` → `[10,100,9]`). Pass `(a,b) => a - b` for numbers; it **mutates** — use `toSorted(cmp)` or `[...arr].sort(cmp)` to stay pure. |
| **The Big Three** | `map` (transform, same length) · `filter` (subset by predicate) · `reduce` (collapse to ANY value — sum, max, object, Map). Reduce = Swiss Army knife: build frequency counts, `groupBy`, flattening, fused filter+map passes. Always pass an initial value; always return the accumulator. |
| **Useful Others** | `find/findIndex/findLast` (first match), `some/every` (any/all), `includes` (finds `NaN`, unlike `indexOf`), `flat(depth)` / `flatMap`, `Array.from(iterable, mapFn)`, `Array.of(...)`, `[...new Set(arr)]` dedupe. |
| **Gotchas** | `sort`/`reverse` mutate AND return the same reference · `new Array(3)` makes holes, not `[3]` · `delete arr[i]` leaves holes (use `splice`) · sparse arrays behave inconsistently across methods · `find` returning `undefined` is ambiguous ("missing" vs "found undefined"). |
| **Related Topics** | objects-and-references (arrays copy by reference) · immutability module (immutability discipline) · closures (callbacks capture state) · Big-O (Set-based intersection vs `includes`) |

### Skeleton
```js
// pipeline grammar: shape -> transform -> collapse
const result = items
  .filter(x => x.active)
  .map(x => x.value)
  .reduce((acc, v) => acc + v, 0);      // always pass an initial value

// numeric sort (comparator contract: negative -> a first)
const asc = [...nums].sort((a, b) => a - b);

// reduce builds anything — e.g. groupBy
const groups = items.reduce((acc, it) => {
  (acc[it.dept] ??= []).push(it);
  return acc;
}, {});
```

### Remember In One Sentence
> **An array is just an object with ordered integer keys — so know which methods mutate in place versus hand you fresh arrays, always give `sort` a comparator, and treat `map`/`filter`/`reduce` as your transform-select-collapse pipeline.**

### Two Facts People Get Wrong
- `sort()` sorts numbers **as strings** by default (`[10, 9, 100]` → `[10, 100, 9]`) and **mutates** the array while returning that same mutated reference.
- `typeof []` is `"object"`, not `"array"` — and `[NaN].indexOf(NaN)` is `-1` while `[NaN].includes(NaN)` is `true`.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer aloud or write it, then check against the README/code.js. A miss means re-read only that section.

1. Why is `typeof []` equal to `"object"`, and which single built-in should you use to test for an array?
2. List three mutating methods and their non-mutating counterparts (including the ES2023 ones).
3. Recite the comparator contract for `sort`: what do negative/positive/zero return values mean?
4. Why does `[10, 9, 100].sort()` produce `[10, 100, 9]`?
5. Write from memory the `groupBy` reduce — accumulator shape, the `??=` line, and why you must `return acc`.
6. What are the two classic reduce bugs related to the initial value and to conditional branches?
7. Why can `includes` find `NaN` but `indexOf` cannot?
8. What is a sparse array, name two ways one gets created accidentally, and why should you avoid them?
