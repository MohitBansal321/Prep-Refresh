# Iterator Pattern — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Behavioral design pattern (GoF). |
| **Intent** | Provide sequential access to a collection's elements *without exposing its underlying representation*, and let the same collection be traversed in different ways. |
| **Problem** | You must loop over collections of wildly different internal shapes (array, tree, paginated API, huge SQL result set, Redis SCAN). Hand-writing loops leaks internals into callers and duplicates/tangles traversal + cursor logic. |
| **Solution** | Extract traversal into a separate stateful cursor object. The collection stays private and offers one factory method — "give me an iterator" — that returns a small object answering `hasNext()` / `next()`. Position lives on the iterator. |
| **Participants** | **Iterator** (defines how to walk: `hasNext`/`next`) · **ConcreteIterator** (tracks position, walks one structure one way) · **Aggregate / IterableCollection** (factory: `createIterator()` / `[Symbol.iterator]()`) · **ConcreteAggregate** (stores elements, produces the iterator). |
| **Flow** | Client asks collection for an iterator → collection builds the matching ConcreteIterator over its private data → client loops `hasNext()` / `next()` → iterator reads element at position, advances, returns it (lazy: may fetch next page now) → repeat until done. Second walk = fresh iterator. |
| **Pros** | Encapsulation preserved (swap array→tree→stream freely) · SRP (storage vs traversal) · multiple traversal orders (fwd/rev, DFS/BFS) · uniform interface across unlike collections · multiple simultaneous independent cursors · laziness → `O(pageSize)` memory, infinite sequences, early exit · OCP (new traversal = new iterator). |
| **Cons** | Overkill for a plain array (just `for...of`) · more moving parts in classic form · stateful cursors fragile under concurrent mutation · laziness hides real cost (N+1 / network per step) · one-shot by default (spent iterator/generator yields nothing on re-loop) · sequential, poor at random access. |
| **Use When** | Hide a collection's internal structure (trees, graphs, custom DS) · same collection needs multiple traversal orders · stream data larger than memory (huge SQL result, S3 listing, Kafka, event log) · wrap a paginated API/DB cursor as a simple stream · lazy or infinite sequences · uniform iteration across unlike collections. |
| **Avoid When** | Plain in-memory array / built-in already iterable · you need frequent random access (`collection[i]`) · tiny fixed collection, traversal never varies · you need the whole dataset in memory anyway (e.g. global sort) · concurrent mutation is the norm and you won't commit to fail-fast/snapshot semantics. |
| **Real Examples** | JS `Array`/`Map`/`Set`/`string` iteration protocol · Node `Readable` streams (`for await`) · `pg-query-stream`, MongoDB `find().cursor()`, Redis `SCAN` · AWS SDK v3 paginators (`paginateScan`) · Azure `PagedAsyncIterableIterator` · TypeORM `stream()` · C# `IAsyncEnumerable` / `await foreach` · LLM token streams (`for await (const chunk of stream)`). |
| **Related Topics** | Composite (Iterator traverses it) · Visitor (acts on elements Iterator supplies) · Observer (push-based dual of pull-based Iterator) · Factory Method (`createIterator()` is one) · Strategy (different iterators ≈ traversal strategies) · Generators & Node streams (idiomatic language-level implementation). |

### Iterable vs Iterator (JS) — do not confuse
- **Iterable** = has `[Symbol.iterator]()` (async: `[Symbol.asyncIterator]()`). The *collection*. Should return a **fresh** iterator each call so it is re-iterable.
- **Iterator** = has `next(): { value, done }`. The *cursor*. Single-pass — once exhausted it stays exhausted.
- A **generator object** (`function*` result) is *both* — but it is a one-shot iterator, so re-looping it yields nothing.

### Native JS forms (what you actually reach for)
- **`Symbol.iterator` + `for...of` / spread `[...x]` / `Array.from`** — implement the protocol, get looping for free.
- **Generators (`function*` / `yield`)** — the compiler builds the position/done state machine. `yield*` delegates to a sub-generator (compose traversals). Lazy → safe for infinite sequences.
- **Async iterators (`Symbol.asyncIterator` + `for await...of`)** — the backend workhorse: stream paginated DB/API results one page at a time, memory `O(pageSize)` not `O(total)`.
- **Multiple simultaneous iterators** — because position lives on the iterator, two loops walk one collection at different speeds (e.g. merging sorted streams).

### Skeleton
```ts
// Classic GoF form
interface Iterator<T> { hasNext(): boolean; next(): T; }
interface IterableCollection<T> { createIterator(): Iterator<T>; }

class Collection<T> implements IterableCollection<T> {
  private items: T[] = [];
  createIterator(): Iterator<T> {           // factory method; array stays private
    let pos = 0;
    return {
      hasNext: () => pos < this.items.length,
      next: () => this.items[pos++],
    };
  }
}

// Idiomatic modern form — a generator IS an iterator + iterable
function* walk<T>(items: T[]): Generator<T> {
  for (const x of items) yield x;           // lazy; yield* to delegate
}

// Async / paginated streaming — the backend star
class PaginatedCursor<T> implements AsyncIterable<T> {
  constructor(private src: { fetchPage(c: string | null): Promise<{ items: T[]; nextCursor: string | null }> }) {}
  async *[Symbol.asyncIterator](): AsyncGenerator<T> {
    let cursor: string | null = null;
    do {
      const page = await this.src.fetchPage(cursor);  // one round-trip per page
      for (const item of page.items) yield item;      // O(pageSize) memory
      cursor = page.nextCursor;
    } while (cursor !== null);
  }
}
// consume: for await (const row of new PaginatedCursor(api)) { ... }
```

### Remember In One Sentence
> **An Iterator is a "Next" button: a small cursor a collection hands out that walks its elements one at a time — pulling each on demand — while the collection keeps its internal storage hidden.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the four participants and give each a one-line responsibility.
2. Where does the *position* live — on the collection or the iterator — and why does that choice matter?
3. What is the difference between an *iterable* and an *iterator* in JavaScript? Which one is a generator object, and what is the catch?
4. Which native JS features are the language's built-in implementation of this pattern? (Name at least three.)
5. What does `function*` / `yield` buy you over hand-writing the GoF classes, and what does `yield*` do?
6. Explain lazy vs eager traversal. Why is an "infinite" generator safe?
7. How do async iterators (`Symbol.asyncIterator` / `for await...of`) let you stream a 10-million-row query without OOM? What is the memory complexity?
8. How do you offer two traversal orders (e.g. DFS and BFS) over one tree without touching the tree's storage?
9. What goes wrong if the collection is mutated mid-iteration, and what are the two accepted semantics for handling it?
10. Iterator vs Observer — which is pull and which is push? And how does Iterator relate to Composite and Visitor?
11. Why is eagerly doing `const all = [...paginatedCursor]` an anti-pattern, and what hidden cost can a single `for await...of` step carry?
12. When is the Iterator pattern the *wrong* choice? (Give at least two situations.)
