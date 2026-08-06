/**
 * ITERATOR PATTERN — Production-style TypeScript example
 * ------------------------------------------------------
 * Intent: Provide a way to access the elements of an aggregate object
 * sequentially WITHOUT exposing its underlying representation, and let the
 * SAME collection be traversed in different ways.
 *
 * This file shows FIVE flavours, from the classic GoF form up to the modern,
 * idiomatic JavaScript/TypeScript forms that you will actually reach for in a
 * NestJS/Node backend:
 *
 *   1. Classic GoF class-based iterator (Iterator + Aggregate interfaces).
 *   2. The SAME collection exposing TWO traversal orders (asc / desc) — proving
 *      traversal is decoupled from storage.
 *   3. Native JS iteration protocol via `Symbol.iterator` + `for...of`.
 *   4. Generator-based iterators (`function*`, `yield`) — the modern way.
 *      Includes two traversals (DFS / BFS) over one tree structure, LAZILY.
 *   5. ASYNC iterator: a paginated API/DB cursor that lazily fetches one page
 *      at a time and is consumed with `for await...of`. This is the single most
 *      useful application of the pattern in backend work: stream a huge result
 *      set without ever holding it all in memory.
 *
 * Run with: ts-node code.ts   (or compile with tsc && node code.js)
 */

// =============================================================================
// PART 1 — CLASSIC GoF ITERATOR
// -----------------------------------------------------------------------------
// Two roles, expressed as interfaces:
//   - Iterator<T>      : knows how to walk elements one at a time.
//   - IterableCollection<T> : knows how to create an iterator over itself.
// The collection NEVER exposes its internal array. Callers get an iterator.
// =============================================================================

/** Iterator role: a cursor that yields elements one at a time. */
export interface Iterator<T> {
  /** True if a subsequent call to next() would return a real element. */
  hasNext(): boolean;
  /** Advance and return the next element. Throws if exhausted. */
  next(): T;
}

/** Aggregate/Collection role: an object that can hand out iterators over itself. */
export interface IterableCollection<T> {
  /** Factory Method: create a fresh, independent iterator. */
  createIterator(): Iterator<T>;
}

/**
 * A domain collection: an ordered list of user IDs. The internal storage
 * (a plain array here) is PRIVATE. Callers may only traverse via an iterator,
 * so we could later swap the array for a linked list, a B-tree, or a paged
 * buffer WITHOUT changing any calling code.
 */
export class UserIdCollection implements IterableCollection<number> {
  private readonly ids: number[] = [];

  add(id: number): void {
    this.ids.push(id);
  }

  /** The only way out: hand back an iterator, not the array. */
  createIterator(): Iterator<number> {
    // ConcreteIterator is defined as an inner class so it can read `this.ids`
    // while keeping that field private to the outside world.
    return new UserIdCollection.ForwardIterator(this.ids);
  }

  /** ConcreteIterator: tracks the current position independently of others. */
  private static ForwardIterator = class implements Iterator<number> {
    private position = 0;
    constructor(private readonly ids: readonly number[]) {}

    hasNext(): boolean {
      return this.position < this.ids.length;
    }

    next(): number {
      if (!this.hasNext()) {
        throw new Error("Iterator exhausted: call hasNext() before next().");
      }
      return this.ids[this.position++];
    }
  };
}

// =============================================================================
// PART 2 — ONE COLLECTION, MULTIPLE TRAVERSALS
// -----------------------------------------------------------------------------
// The real power: the collection stays the same; we vary HOW we walk it by
// returning a different ConcreteIterator. Storage and traversal are decoupled.
// =============================================================================

export class Playlist implements IterableCollection<string> {
  private readonly tracks: string[] = [];

  add(track: string): void {
    this.tracks.push(track);
  }

  /** Default forward traversal. */
  createIterator(): Iterator<string> {
    return new ArrayCursor(this.tracks, "forward");
  }

  /** A second traversal order over the SAME data. */
  createReverseIterator(): Iterator<string> {
    return new ArrayCursor(this.tracks, "reverse");
  }
}

/** A reusable array cursor supporting two directions. */
class ArrayCursor<T> implements Iterator<T> {
  private position: number;

  constructor(
    private readonly items: readonly T[],
    private readonly direction: "forward" | "reverse",
  ) {
    this.position = direction === "forward" ? 0 : items.length - 1;
  }

  hasNext(): boolean {
    return this.direction === "forward"
      ? this.position < this.items.length
      : this.position >= 0;
  }

  next(): T {
    if (!this.hasNext()) throw new Error("Iterator exhausted.");
    const value = this.items[this.position];
    this.position += this.direction === "forward" ? 1 : -1;
    return value;
  }
}

// =============================================================================
// PART 3 — NATIVE JS ITERATION PROTOCOL (Symbol.iterator + for...of)
// -----------------------------------------------------------------------------
// JavaScript has the Iterator pattern built into the language. An object is
// "iterable" if it has a [Symbol.iterator]() method returning an object with a
// next(): { value, done } method. Implementing it unlocks for...of, spread
// (`[...x]`), destructuring, Array.from, Map/Set construction, etc. — for free.
// =============================================================================

/**
 * A ring buffer (fixed-size circular queue) that is natively iterable. Because
 * it implements the protocol, callers use `for (const x of buffer)` and never
 * touch the internal array or head/tail pointers.
 */
export class RingBuffer<T> implements Iterable<T> {
  private readonly store: (T | undefined)[];
  private head = 0;
  private count = 0;

  constructor(private readonly capacity: number) {
    this.store = new Array<T | undefined>(capacity);
  }

  push(item: T): void {
    const tail = (this.head + this.count) % this.capacity;
    this.store[tail] = item;
    if (this.count < this.capacity) this.count++;
    else this.head = (this.head + 1) % this.capacity; // overwrite oldest
  }

  /**
   * The protocol hook. Returning a fresh iterator each time means multiple
   * independent `for...of` loops (even nested) work correctly.
   */
  [Symbol.iterator](): globalThis.Iterator<T> {
    let visited = 0;
    let index = this.head;
    const { store, count, capacity } = this;

    return {
      next(): IteratorResult<T> {
        if (visited >= count) {
          return { value: undefined as unknown as T, done: true };
        }
        const value = store[index] as T;
        index = (index + 1) % capacity;
        visited++;
        return { value, done: false };
      },
    };
  }
}

// =============================================================================
// PART 4 — GENERATOR-BASED ITERATORS (function*, yield)
// -----------------------------------------------------------------------------
// A generator function (`function*`) returns an object that is BOTH an iterator
// and iterable. `yield` produces one value and pauses execution until the
// consumer asks for the next one. This is the idiomatic modern way to write an
// iterator: the compiler builds the state machine (position, done flag) for you.
//
// Below: ONE tree, TWO lazy traversals (depth-first and breadth-first). Only the
// traversal function differs; the tree structure is untouched.
// =============================================================================

export class TreeNode<T> {
  readonly children: TreeNode<T>[] = [];
  constructor(public readonly value: T) {}
  addChild(child: TreeNode<T>): this {
    this.children.push(child);
    return this;
  }
}

/** Depth-first pre-order traversal, produced lazily one node at a time. */
export function* depthFirst<T>(root: TreeNode<T>): Generator<T> {
  yield root.value;
  for (const child of root.children) {
    // Delegation: `yield*` forwards every value from the recursive generator.
    yield* depthFirst(child);
  }
}

/** Breadth-first traversal over the SAME tree, also lazy. */
export function* breadthFirst<T>(root: TreeNode<T>): Generator<T> {
  const queue: TreeNode<T>[] = [root];
  while (queue.length > 0) {
    const node = queue.shift()!;
    yield node.value;
    queue.push(...node.children);
  }
}

/**
 * Generators shine for INFINITE / very large sequences because they are lazy:
 * nothing is computed until requested, so an "infinite" iterator is fine as
 * long as the consumer stops pulling. Here: an unbounded ID generator.
 */
export function* infiniteIds(start = 1): Generator<number> {
  let id = start;
  while (true) {
    yield id++;
  }
}

/** A lazy `take` combinator — pull only N elements from any iterable. */
export function take<T>(iterable: Iterable<T>, n: number): T[] {
  const out: T[] = [];
  for (const item of iterable) {
    if (out.length >= n) break;
    out.push(item);
  }
  return out;
}

// =============================================================================
// PART 5 — ASYNC ITERATOR: A LAZY PAGINATED CURSOR
// -----------------------------------------------------------------------------
// THE backend use case. A repository/HTTP API returns results one PAGE at a
// time (keyset/cursor pagination). We expose the whole result set as an ASYNC
// iterable: each `for await...of` step transparently fetches the next page only
// when the current page is exhausted. Memory stays O(pageSize), not O(total),
// so you can stream millions of rows without OOM-ing the process.
// =============================================================================

/** One page of results plus a cursor to fetch the following page. */
export interface Page<T> {
  items: T[];
  nextCursor: string | null; // null => no more pages
}

/** Port: anything that can fetch a page given an optional cursor. */
export interface PagedSource<T> {
  fetchPage(cursor: string | null): Promise<Page<T>>;
}

/**
 * Wraps a PagedSource and exposes it as a normal async-iterable stream. The
 * caller writes `for await (const row of cursor) { ... }` and is completely
 * unaware of page boundaries, cursors, or network calls.
 */
export class PaginatedCursor<T> implements AsyncIterable<T> {
  constructor(
    private readonly source: PagedSource<T>,
    private readonly maxItems = Infinity, // optional safety cap
  ) {}

  async *[Symbol.asyncIterator](): AsyncGenerator<T> {
    let cursor: string | null = null;
    let emitted = 0;

    do {
      // Lazily fetch exactly one page — the only network/DB round-trip.
      const page: Page<T> = await this.source.fetchPage(cursor);

      for (const item of page.items) {
        if (emitted >= this.maxItems) return;
        yield item;
        emitted++;
      }

      cursor = page.nextCursor;
    } while (cursor !== null);
  }
}

/**
 * A fake paginated data source simulating a Postgres keyset query or an HTTP
 * API. It "stores" `total` rows and serves them `pageSize` at a time, with a
 * small delay to mimic I/O latency. We cannot load all rows at once in real
 * life — that is exactly why we iterate.
 */
export class FakeUserApi implements PagedSource<{ id: number; name: string }> {
  constructor(
    private readonly total: number,
    private readonly pageSize: number,
  ) {}

  async fetchPage(
    cursor: string | null,
  ): Promise<Page<{ id: number; name: string }>> {
    const offset = cursor ? Number(cursor) : 0;
    await delay(5); // simulate network/DB latency

    const items = Array.from(
      { length: Math.min(this.pageSize, this.total - offset) },
      (_, i) => ({ id: offset + i, name: `user-${offset + i}` }),
    );

    const next = offset + this.pageSize;
    console.log(
      `  [FakeUserApi] served page offset=${offset} size=${items.length}`,
    );
    return { items, nextCursor: next < this.total ? String(next) : null };
  }
}

function delay(ms: number): Promise<void> {
  return new Promise((resolve) => setTimeout(resolve, ms));
}

// =============================================================================
// DEMO — each part runs identically for the consumer, regardless of internals.
// =============================================================================

async function main(): Promise<void> {
  // ---- Part 1: classic iterator ----
  console.log("=== Part 1: Classic GoF iterator ===");
  const users = new UserIdCollection();
  [101, 102, 103].forEach((id) => users.add(id));
  const it = users.createIterator();
  while (it.hasNext()) console.log("user id:", it.next());

  // ---- Part 2: two traversals, one collection ----
  console.log("\n=== Part 2: Same collection, two traversals ===");
  const playlist = new Playlist();
  ["intro", "verse", "chorus"].forEach((t) => playlist.add(t));
  const fwd = playlist.createIterator();
  const rev = playlist.createReverseIterator();
  const fwdOrder: string[] = [];
  const revOrder: string[] = [];
  while (fwd.hasNext()) fwdOrder.push(fwd.next());
  while (rev.hasNext()) revOrder.push(rev.next());
  console.log("forward:", fwdOrder.join(" -> "));
  console.log("reverse:", revOrder.join(" -> "));

  // ---- Part 3: native protocol ----
  console.log("\n=== Part 3: Native Symbol.iterator + for...of ===");
  const buffer = new RingBuffer<number>(3);
  [1, 2, 3, 4, 5].forEach((n) => buffer.push(n)); // 1,2 overwritten
  console.log("ring buffer (for...of):", [...buffer].join(", ")); // spread also works

  // ---- Part 4: generators (DFS/BFS + lazy infinite) ----
  console.log("\n=== Part 4: Generator traversals over one tree ===");
  const root = new TreeNode("A");
  const b = new TreeNode("B");
  const c = new TreeNode("C");
  root.addChild(b).addChild(c);
  b.addChild(new TreeNode("D")).addChild(new TreeNode("E"));
  c.addChild(new TreeNode("F"));
  console.log("DFS:", [...depthFirst(root)].join(" "));
  console.log("BFS:", [...breadthFirst(root)].join(" "));
  console.log("first 5 of an INFINITE id stream:", take(infiniteIds(1000), 5));

  // ---- Part 5: async paginated cursor ----
  console.log("\n=== Part 5: Async paginated cursor (for await...of) ===");
  const api = new FakeUserApi(/* total */ 12, /* pageSize */ 5);
  const cursor = new PaginatedCursor(api);

  let seen = 0;
  for await (const user of cursor) {
    seen++;
    if (user.id % 4 === 0) {
      console.log(`  processing ${user.name}`);
    }
  }
  console.log(`streamed ${seen} users while holding <= 5 in memory at a time.`);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
