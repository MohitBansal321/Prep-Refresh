# Arrays and Array Methods — Exercises

Work through these in order. They are deliberately **solution-free**: the point is
to build reflexes — writing `map`/`reduce` from raw loops until you no longer need
to look up their signatures, and knowing *why* each array method does what it does.

> Rule of thumb for every exercise: state out loud whether your function mutates
> its input. If it does, ask whether a non-mutating version is more appropriate
> (in interviews, "this one mutates, here's why I chose that" is half the credit).

---

## Easy 1 — Implement `map` From Scratch

Write `myMap(array, callback)` using only a plain `for` loop and `push`.

**Requirements:**
- Callback receives `(element, index, array)` — all three, like the real one.
- Returns a **new** array; never touches the input.
- Handles empty arrays (returns `[]`).

**Acceptance:**
- `myMap([1, 2, 3], n => n * 2)` → `[2, 4, 6]`.
- `myMap(["a", "b"], (s, i) => s + i)` → `["a0", "b1"]`.
- After calling it, the original array is unchanged (verify with a log).

**Reflection:** if someone replaced your loop body with `array[i] = callback(...)` instead of pushing to a new array, what two things would break?

---

## Easy 2 — Implement `filter` From Scratch

Same deal: `myFilter(array, predicate)` with a plain loop.

**Requirements:**
- Predicate returns truthy/falsy — use its *truthiness*, don't force `=== true`.
- Returns a new array of only the passing elements, preserving order.

**Acceptance:**
- `myFilter([1, 2, 3, 4], n => n % 2 === 0)` → `[2, 4]`.
- `myFilter([0, "", "a", null], x => x)` → `["a"]` (truthy filter works).

**Reflection:** why did the last acceptance case work even though `"a"` isn't a boolean? What does that tell you about how `filter`'s real implementation decides?

---

## Medium 1 — Implement `reduce` From Scratch

Write `myReduce(array, callback, initialValue)`. This is the one that separates
"uses reduce" from "understands reduce".

**Requirements:**
- Callback receives `(accumulator, current, index, array)`.
- If `initialValue` is provided, start from it; otherwise seed with element 0 and
  begin iterating at index 1.
- Throw the same style of error as native reduce when there's no initial value
  **and** the array is empty.

**Acceptance:**
- `myReduce([1, 2, 3], (a, b) => a + b, 0)` → `6`.
- `myReduce([1, 2, 3], (a, b) => a + b)` → `6` (no initial value path).
- `myReduce([], (a, b) => a + b)` throws; `myReduce([], f, 10)` → `10`.

**Reflection:** in one sentence, why does the no-initial-value version throw on an empty array while the with-initial-value version happily returns the seed?

---

## Medium 2 — `chunk(array, size)`

Split an array into sub-arrays of at most `size` elements.

**Requirements:**
- Non-mutating; use `slice`.
- Last chunk may be smaller than `size`; never produce empty chunks.
- Reject/throw on `size < 1`.

**Acceptance:**
- `chunk([1, 2, 3, 4, 5], 2)` → `[[1, 2], [3, 4], [5]]`.
- `chunk([1, 2], 5)` → `[[1, 2]]`.
- `chunk([], 3)` → `[]`.

**Reflection:** your loop condition is probably `i += size`. Why does stepping by `size` (not by 1) still visit every element exactly once?

---

## Medium 3 — `intersection(a, b)` and `difference(a, b)`

**Requirements:**
- Both return new arrays, both dedupe their result, both preserve first-seen order.
- First implement the naive version with `includes`, then rewrite using a `Set`
  and explain the complexity change (O(n·m) → O(n + m)).

**Acceptance:**
- `intersection([1, 2, 2, 3], [2, 3, 4])` → `[2, 3]`.
- `difference([1, 2, 2, 3], [2])` → `[1, 3]`.

**Reflection:** why does converting only `b` to a Set (and not `a`) already give you the big win? What would you convert if the inputs were swapped?

---

## Hard 1 — `groupBy(collection, keyFn)`

Implement `groupBy(array, keyFn)` returning a plain object mapping keys to arrays
of elements. This is the single most common "write it cold" interview reduce.

**Requirements:**
- `keyFn(element)` computes the group key per element.
- Works for any key type `keyFn` produces (strings at minimum).
- Do NOT mutate the input; elements are shared by reference into groups (that's fine — say why).

**Acceptance:**
- Grouping `["ant", "bee", "ape"]` by `s => s[0]` → `{ a: ["ant", "ape"], b: ["bee"] }`.
- Grouping people objects by `dept` yields every person appearing in exactly one group.

**Reflection:** implement it twice — once with `reduce`, once with a plain `for...of` loop. Which reads better? Under what team norms would you defend the reduce version?

---

## Hard 2 — One-Pass Second Largest

Find the second-largest **distinct** value in an array without sorting.

**Requirements:**
- Single pass (`reduce` or a loop), O(1) extra space, handles duplicates correctly.
- Return `-Infinity` (or document your choice) when fewer than 2 distinct values exist.

**Acceptance:**
- `[5, 1, 9, 9, 3]` → `5`.
- `[7, 7]` → `-Infinity`.
- `[-2, -8, -1]` → `-2`.

**Reflection:** the obvious alternative is `[...arr].toSorted((a,b) => b-a)[1]`. Compare the two approaches on time, space, mutation-safety, and readability — when would you actually pick the sort version?

---

## Hard 3 — Flatten Arbitrarily Deep, By Hand

Write `deepFlatten(array)` without using `flat(Infinity)`.

**Requirements:**
- Handle any nesting depth, mixed with non-array leaves.
- Choose recursion or an explicit stack; either is fine — be ready to justify it.

**Acceptance:**
- `deepFlatten([1, [2, [3, [4]], 5]])` → `[1, 2, 3, 4, 5]`.
- `deepFlatten([])` → `[]`; `deepFlatten([[[]]])` → `[]`.

**Reflection:** what happens with a self-referencing array (`const a = [1]; a.push(a); deepFlatten(a)`)? Name the problem and sketch (words, not code) one way to guard against it.

---

*No solutions are provided on purpose. When you finish an attempt, compare it against `code.js` and the README sections, or ask for a guided review.*
