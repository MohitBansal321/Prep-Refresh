# Iterator Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of separating *how you walk a collection* from *how the collection stores its data*, and to become fluent with generators and async iterators — the forms you will actually use in backend code.

> Rule of thumb for every exercise: the collection's internal storage must stay **private**. Callers may only traverse through an iterator / `for...of` / `for await...of`. Position lives on the **iterator**, never on the collection.

---

## Easy — Range Iterator (classic form)

Implement the classic GoF form for a numeric range.

```ts
interface Iterator<T> {
  hasNext(): boolean;
  next(): T;
}
interface IterableCollection<T> {
  createIterator(): Iterator<T>;
}
```

**Task:** Write a `NumberRange` class constructed with `(start, end, step)` that implements `IterableCollection<number>`. Its `createIterator()` returns an iterator that yields `start, start+step, ...` up to (but not including) `end`. The range must **not** store all the numbers in an array — the iterator computes each value on demand.

**Acceptance:** For `new NumberRange(0, 10, 2)`, walking the iterator yields `0, 2, 4, 6, 8`. Two iterators created from the same range advance independently.

---

## Medium — Make It Native, Then a Generator

Take a `WordBag` collection that stores words in a private array.

**Part A:** Implement `[Symbol.iterator]()` so that `for (const w of bag)`, `[...bag]`, and `Array.from(bag)` all work. Do **not** expose the internal array.

**Part B:** Re-implement the same traversal using a **generator method** (`*[Symbol.iterator]()` with `yield`). Compare how much less code the generator version needs.

**Part C:** Add a second generator method `reversed()` that yields the words in reverse order — proving one collection can offer multiple traversals without touching storage.

**Bonus constraint:** Add a generator `unique()` that yields each distinct word only once, lazily (do not build a full `Set` of everything up front if you can avoid it — yield as you discover new words).

---

## Hard — Lazy Iterator Combinators (a mini "lazy stream" library)

Build composable, lazy operators that work over **any** `Iterable<T>`, pulling elements one at a time (no intermediate arrays).

**Task:** Implement these as generator functions:
- `map<T, U>(src: Iterable<T>, fn: (x: T) => U): Iterable<U>`
- `filter<T>(src: Iterable<T>, pred: (x: T) => boolean): Iterable<T>`
- `take<T>(src: Iterable<T>, n: number): Iterable<T>`
- `zip<A, B>(a: Iterable<A>, b: Iterable<B>): Iterable<[A, B]>`

**Requirements:**
- Everything must be **lazy**: `take(map(infinite(), x => x*2), 3)` must terminate even though `infinite()` never ends.
- No operator may materialize its input into an array.
- Prove laziness by counting how many times the `map` function actually runs when wrapped in `take(_, 3)` — it must be exactly 3.

**Think about:** why pull-based laziness gives you early exit "for free", and how this compares to array methods (`arr.map().filter().slice()`), which are eager and allocate an array at each step.

---

## Real-World Challenge — Paginated Database Cursor

Model streaming a large result set out of a data source, as you would in a NestJS service.

```ts
interface Page<T> { items: T[]; nextCursor: string | null; }
interface PagedSource<T> { fetchPage(cursor: string | null): Promise<Page<T>>; }
```

**Task:**
1. Implement a `Repository`-style fake `PagedSource<Order>` backed by an in-memory array of ~1000 orders, serving `pageSize` at a time using **keyset** semantics (the cursor is the last seen `id`, not an offset). Add a small simulated latency.
2. Implement `class OrderCursor implements AsyncIterable<Order>` that lazily fetches the next page only when the current one is exhausted (`async *[Symbol.asyncIterator]()`).
3. Write a consumer `exportOrders(cursor)` that uses `for await (const order of cursor)` to process every order and returns the count — while never holding more than one page in memory.
4. Add a `maxItems` safety cap so a caller can stream "the first N" and stop early (the cursor must **not** fetch pages it does not need).

**Requirements:**
- The consumer code must contain **no** mention of pages, cursors, or `fetchPage`.
- Log each page fetch and confirm from the logs that pages are fetched on demand (and that early exit stops fetching).
- Write one unit test that swaps in an in-memory fake `PagedSource` with a known dataset and asserts the streamed sequence — no real I/O.

**Stretch:** Add a `batch(size)` async-generator combinator that regroups the per-element async stream into arrays of `size` (e.g. to bulk-insert into another table). This shows re-shaping a stream without buffering it all.

---

## Bonus Challenge — Traversals, Semantics, and Merge

1. **One tree, three walks.** Given an n-ary `TreeNode<T>`, implement three generator traversals over the *same* structure: pre-order DFS, post-order DFS, and BFS. Use `yield*` delegation for the recursive ones. The tree class itself must not contain any traversal logic.

2. **Fail-fast semantics.** Add a `modCount` version counter to a mutable `SafeList<T>`. Its iterator must snapshot `modCount` at creation and **throw** a `ConcurrentModificationError` if `next()` is called after the list was mutated. Then implement a second collection `SnapshotList<T>` whose iterator copies the data at creation so mutations are invisible to an in-flight walk. Write down when you would choose each.

3. **Merge two sorted streams.** Given two *already sorted* iterables, implement a lazy `mergeSorted(a, b)` generator that yields all elements in sorted order by pulling from whichever iterator currently has the smaller head — demonstrating **two simultaneous, independent cursors**. Confirm it works on infinite sorted generators combined with `take`.

4. **Pull meets push.** Briefly (in comments) contrast your `OrderCursor` (pull-based async iterator) with how you would model the same data as an RxJS `Observable` (push-based). When does backpressure become your problem in each model?

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
