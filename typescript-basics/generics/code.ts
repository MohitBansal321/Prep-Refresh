/**
 * GENERICS — Production-style TypeScript example
 * ------------------------------------------------
 * Scenario: Our application needs a data-access layer that behaves identically
 * for every entity (User, Product, ...): findById, save, getAll. Writing one
 * repository class per entity would duplicate the same logic forever; typing
 * everything as `any` would compile but throw away all type safety.
 *
 * We solve this with Generics:
 *   - identity<T> / firstOrDefault<T>   -> plain generic functions
 *   - HasId + getIds<T extends HasId>   -> a generic constraint
 *   - Repository<T, TCreate = Partial<T>> -> a generic interface with a default type param
 *   - InMemoryRepository<T, TCreate>    -> a generic class, instantiated for two unrelated types
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. GENERIC FUNCTIONS — placeholder type T, no constraint needed.
//    The same code works for numbers, strings, objects, anything at all.
// =============================================================================

/** The simplest possible generic: whatever type goes in comes back out, unchanged. */
export function identity<T>(x: T): T {
  return x;
}

/**
 * Returns the first element of an array, or a fallback if the array is empty.
 * T is inferred from whatever array (and fallback) is passed in — the caller
 * never has to write out the type argument explicitly.
 */
export function firstOrDefault<T>(items: T[], fallback: T): T {
  return items.length > 0 ? items[0] : fallback;
}

// =============================================================================
// 2. GENERIC CONSTRAINT — <T extends HasId> restricts T to "anything with an id"
//    without collapsing back to `any`. This is the minimum permission needed.
// =============================================================================

/** Any type satisfying this shape can be used wherever a constrained T is required. */
export interface HasId {
  id: string;
}

/**
 * Extracts the ids from a list of entities. Works for User[], Product[], or any
 * future entity type — as long as it has an `id: string`. Calling this with,
 * say, a number[] is a COMPILE-TIME error, not a runtime surprise:
 *
 *   getIds([1, 2, 3]); // Error: number does not satisfy constraint HasId
 */
export function getIds<T extends HasId>(items: T[]): string[] {
  return items.map((item) => item.id);
}

// =============================================================================
// 3. GENERIC INTERFACE — the contract our application designs, parameterized
//    over the entity type T. TCreate has a DEFAULT type parameter, so most
//    callers can omit it entirely.
// =============================================================================

/**
 * TCreate defaults to Partial<T> because "the data needed to create a T" is,
 * in the common case, just T without its generated `id`. Callers with unusual
 * creation requirements can still override TCreate explicitly.
 */
export interface Repository<T extends HasId, TCreate = Partial<T>> {
  findById(id: string): T | undefined;
  getAll(): T[];
  save(data: TCreate): T;
}

// =============================================================================
// 4. GENERIC CLASS — one implementation, instantiated per entity type.
//    All storage/lookup logic is written ONCE here.
// =============================================================================

export class InMemoryRepository<T extends HasId, TCreate = Partial<T>>
  implements Repository<T, TCreate>
{
  private readonly items = new Map<string, T>();
  private nextId = 1;

  findById(id: string): T | undefined {
    return this.items.get(id);
  }

  getAll(): T[] {
    return [...this.items.values()];
  }

  save(data: TCreate): T {
    // In a real repository this would hit a database; here we simulate an
    // auto-generated id and merge it with the caller-supplied fields.
    const id = `id-${this.nextId++}`;
    // Double cast through `unknown`: TypeScript cannot prove that `{ id } & TCreate`
    // satisfies the arbitrary constrained T, even though at runtime it always will
    // for any well-formed TCreate. This is the one place we assert what the
    // generic constraint guarantees but the structural checker cannot verify.
    const entity = { id, ...data } as unknown as T;
    this.items.set(id, entity);
    return entity;
  }
}

// =============================================================================
// 5. DOMAIN TYPES — two intentionally unrelated entities, both satisfying
//    HasId. Nothing about Repository/InMemoryRepository changes for either.
// =============================================================================

export interface User extends HasId {
  name: string;
  email: string;
}

export interface Product extends HasId {
  title: string;
  priceInCents: number;
}

// =============================================================================
// 6. DEMO — same generic definitions, two completely different concrete types.
//    Notice: zero duplicated logic, full type safety, full autocomplete.
// =============================================================================

function main(): void {
  // --- Plain generics ---
  console.log("identity(42):", identity(42)); // T inferred as number
  console.log("identity('hello'):", identity("hello")); // T inferred as string
  console.log(
    "firstOrDefault([], 'none'):",
    firstOrDefault<string>([], "none"), // explicit type argument, nothing to infer from
  );

  // --- Generic class, instantiation #1: User ---
  const userRepo = new InMemoryRepository<User>();
  const asha = userRepo.save({ name: "Asha", email: "asha@example.com" });
  const vikram = userRepo.save({ name: "Vikram", email: "vikram@example.com" });
  console.log("Saved users:", userRepo.getAll());
  console.log("findById(asha.id):", userRepo.findById(asha.id));

  // --- Generic class, instantiation #2: Product ---
  // Same class, same logic, completely different (and unrelated) entity type.
  const productRepo = new InMemoryRepository<Product>();
  const keyboard = productRepo.save({ title: "Mechanical Keyboard", priceInCents: 8999 });
  const mouse = productRepo.save({ title: "Wireless Mouse", priceInCents: 2499 });
  console.log("Saved products:", productRepo.getAll());

  // --- Generic constraint in action ---
  console.log("User ids:", getIds(userRepo.getAll())); // T inferred as User
  console.log("Product ids:", getIds(productRepo.getAll())); // T inferred as Product

  // The following would NOT compile — plain numbers do not satisfy HasId:
  // getIds([1, 2, 3]);

  void [vikram, mouse]; // keep TS happy about "unused" demo values
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
