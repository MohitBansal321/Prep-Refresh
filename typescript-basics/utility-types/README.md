# Built-in Utility Types

## Intent

Derive a new type from an existing type — adding, removing, or transforming properties — without hand-writing a second, parallel interface that has to be kept in sync by hand.

## Real Life Analogy

Imagine you have a master architectural blueprint for a house — every room, every wall, every pipe. Now three different people need three different views of that same house:

- The **real-estate agent** only wants a floor-plan showing room names and square footage (no plumbing, no wiring).
- The **plumber** wants the exact same blueprint but with every room made "read-only" — he is not allowed to redesign the layout, only look at where the pipes are.
- The **renovation contractor** wants the blueprint with every field *optional*, because a renovation request might only touch the kitchen and leave everything else untouched.

You do not draft three unrelated blueprints from scratch — that guarantees they drift apart the moment the house changes. Instead you take the *one* master blueprint and mechanically **transform** it: hide some rooms, freeze some rooms, make some rooms optional. TypeScript's built-in utility types are exactly this: mechanical transformations of one source type into the derived shape a particular consumer needs.

## Problem

### What engineering problem exists?

Real applications constantly need several *shapes* of the same underlying entity:

- A `User` stored in the database has `passwordHash` — but the shape sent back to the client in an API response must never include it.
- A `PATCH /users/:id` endpoint should accept a body where every field is optional (the caller only sends what they want to change), but a `POST /users` endpoint requires all fields.
- A public profile page shows only `id`, `name`, and `email` — nothing else.
- A permissions table needs a lookup keyed by every possible role, guaranteeing (at compile time) that no role is ever forgotten.
- A helper function needs "whatever type some factory function returns" without re-typing that return shape by hand.

Every one of these is *the same base type*, viewed through a different lens.

> **Term: Derived type.** A derived type is a type computed *from* another type (the "source" or "base" type) rather than written out independently. If the source type changes, a well-built derived type changes with it automatically.

### Why is this problem difficult?

- **Hand-duplicating types silently drift.** If you write `UpdateUserDto` as a brand-new interface with the same fields as `User` but all optional, nothing forces the two to stay in sync. Add a field to `User` and forget to add it to `UpdateUserDto`, and the compiler will not warn you — the bug surfaces at runtime instead.
- **"Optional everywhere" vs "required everywhere" needs to be expressed structurally**, not by retyping every field with a `?`. Doing this by hand for a 15-field interface is tedious and error-prone.
- **Excluding one sensitive field (like `passwordHash`) by hand** means re-listing every *other* field — the opposite of DRY, and a new field you add to `User` will leak into the API response type unless you remember to also add it to the hand-written "safe" type.
- **Function-shape types (return values, parameter tuples) are often anonymous** — a factory function's return type may be an inline object literal with no name at all, so there is nothing to "import and reuse" unless the language gives you a way to ask "what does this function return?"

### What happens if we ignore it?

- **Type drift.** Hand-written "shadow" types (`CreateUserDto`, `UpdateUserDto`, `PublicUser`, ...) silently diverge from the real `User` type as the codebase evolves, and the compiler cannot catch it because they were never actually *linked*.
- **Accidental data leaks.** Forgetting to strip `passwordHash` (or a token, or an internal flag) from an API response type is a security bug, not just a style nit.
- **Repetition fatigue leads to `any`.** Faced with retyping a large interface for the fifth slightly-different variant, developers reach for `any` or `Record<string, any>`, throwing away type safety entirely.
- **Refactors become risky.** Renaming or removing a field on the source type should ripple through every place that depends on it. Hand-duplicated types do not ripple — they just quietly become wrong.

## Why Not Other Solutions?

**"Just write a second interface by hand."**
Works for a moment, then rots. Nothing ties `UpdateUserDto` back to `User`; a change to one is never enforced on the other.

**"Use `any` or `Record<string, unknown>` for the derived shape."**
This throws away the entire reason you are using TypeScript. You lose autocomplete, you lose compile-time checks, and typos in property names go undetected.

**"Copy-paste the interface and tweak it."**
This is the classic "shotgun surgery" trap: one conceptual change (add a field to `User`) now requires manually finding and editing every copy, and it is easy to miss one.

**"Write your own generic mapped type for every situation from scratch."**
This *is* essentially what the built-in utility types already are — `Partial`, `Pick`, `Omit`, etc. are themselves tiny mapped/conditional types shipped in `lib.es5.d.ts`. Reinventing them by hand for every project duplicates effort TypeScript has already solved generically and given a name to.

**Tradeoff summary:** Every alternative either duplicates information that should have one source of truth, or discards type safety. Built-in utility types solve this by **transforming** the source type mechanically — the derived type is always structurally tied to the source, so if the source changes, the derived type changes automatically the next time you compile.

## Solution

The core idea: **express the derived shape as a *transformation* of the source type, using generics, so the compiler recomputes it every time the source changes.**

TypeScript ships a standard library of these transformations as **generic utility types** — `Partial<T>`, `Required<T>`, `Readonly<T>`, `Pick<T, K>`, `Omit<T, K>`, `Record<K, V>`, `ReturnType<T>`, `Parameters<T>`, `Exclude<T, U>`, `Extract<T, U>`, and others. You reach for the one that matches the transformation you need instead of hand-writing a new interface.

The thinking behind it:

1. **One source of truth.** Define the real-world entity (`User`) once. Every other shape (`UpdateUserDto`, `PublicUser`, `SafeUser`, ...) is *computed* from it.
2. **Let the compiler keep things in sync.** Because `Omit<User, "passwordHash">` is a live computation over `User`'s properties, adding a new field to `User` automatically flows into every type derived from it — no manual edits needed.
3. **Name the transformation, not the result.** `Partial<User>` documents *why* the type exists ("all fields optional, for a partial update") far better than a differently-named interface with no visible connection to `User` would.

You do **not** hand-maintain parallel interfaces. You **derive**.

## How They're Built: Mapped Types and Conditional Types

This is the part most people skip, and it is the most useful part to understand: **none of these utility types are special compiler magic.** Every single one of them is ordinary TypeScript, built from two generic-programming features you can use yourself:

> **Term: Mapped type.** A type of the form `{ [K in keyof T]: SomeTransform<T[K]> }` — it iterates over every key `K` of `T` and produces a new property for each one, optionally transforming the value type or the modifiers (`?`, `readonly`).

> **Term: Conditional type.** A type of the form `T extends U ? X : Y` — it branches on whether one type is assignable to another, evaluated per-member when combined with a union (this is called *distributive conditional type*).

Here is what a few of the "magic" utility types actually look like inside TypeScript's own standard library (`lib.es5.d.ts`):

```ts
// Partial<T> — a mapped type that adds "?" to every property
type Partial<T> = { [K in keyof T]?: T[K] };

// Required<T> — a mapped type that REMOVES "?" from every property
type Required<T> = { [K in keyof T]-?: T[K] };

// Readonly<T> — a mapped type that adds "readonly" to every property
type Readonly<T> = { readonly [K in keyof T]: T[K] };

// Pick<T, K> — a mapped type that keeps only the keys listed in K
type Pick<T, K extends keyof T> = { [P in K]: T[P] };

// Record<K, V> — a mapped type that builds an object type from scratch
type Record<K extends keyof any, V> = { [P in K]: V };

// Exclude<T, U> — a conditional type that drops union members assignable to U
type Exclude<T, U> = T extends U ? never : T;

// Omit<T, K> — Pick combined with Exclude: keep every key NOT in K
type Omit<T, K extends keyof any> = Pick<T, Exclude<keyof T, K>>;
```

Once you see this, the "built-in utility types" stop looking like a fixed list to memorize and start looking like *examples of a technique*. If none of the shipped utilities fit your exact need, you write your own mapped or conditional type the same way — this is precisely how libraries define things like `DeepPartial<T>` or `Nullable<T>`.

## The Utility Types, One By One

All examples below transform this one source type:

```ts
type UserRole = "admin" | "editor" | "viewer";

interface User {
  id: string;
  name: string;
  email: string;
  passwordHash: string;
  role: UserRole;
  createdAt: Date;
}
```

**`Partial<T>`** — makes every property optional.
*Use case:* `Partial<User>` as the body type for `PATCH /users/:id` — the caller may send only the fields they want to change (`{ name: "New Name" }` is valid, and so is `{ email: "..." }`).

**`Required<T>`** — makes every property mandatory (removes `?`).
*Use case:* if `User` had optional fields like `avatarUrl?`, `Required<User>` describes the shape *after* you have applied defaults for every optional field — e.g. the return type of a "hydrate with defaults" function that guarantees nothing is left `undefined`.

**`Readonly<T>`** — makes every property immutable after creation.
*Use case:* `Readonly<User>` for an entity handed out from an in-memory cache — nothing downstream should be able to mutate the cached object directly; they must go through an update function instead.

**`Pick<T, K>`** — keeps only the listed keys.
*Use case:* `Pick<User, "id" | "name" | "email">` as `PublicProfile` — the shape shown on a public profile page, with `passwordHash`, `role`, and `createdAt` deliberately excluded by construction.

**`Omit<T, K>`** — keeps every key *except* the listed ones.
*Use case:* `Omit<User, "passwordHash">` as `SafeUser` — the shape returned from every API response. Because it is computed as "everything except `passwordHash`," any new field later added to `User` is included automatically, and the one field that must never leak is guaranteed excluded.

**`Record<K, V>`** — builds an object type mapping every member of key-union `K` to value type `V`.
*Use case:* `Record<UserRole, Permission[]>` as a permissions lookup table. Because `K` is the literal union `"admin" | "editor" | "viewer"`, the compiler *forces* you to provide an entry for every role — forgetting one is a compile error, not a runtime surprise.

**`ReturnType<T>`** — extracts the return type of a function type.
*Use case:* `ReturnType<typeof createUser>` where `createUser` is a factory function. If the factory's return shape changes, every place using `ReturnType<typeof createUser>` updates automatically — no separately-declared `User`-like type to fall out of sync.

**`Parameters<T>`** — extracts a function's parameter types as a tuple.
*Use case:* `Parameters<typeof createUser>` when you write a generic logging/wrapper function (`function logged<F extends (...args: any[]) => any>(fn: F, ...args: Parameters<F>) { ... }`) that must accept exactly the arguments `createUser` accepts, without retyping them.

**`Exclude<T, U>` / `Extract<T, U>`** (briefly) — filter a union type.
`Exclude<UserRole, "viewer">` removes `"viewer"` from the role union, leaving `"admin" | "editor"` (useful for a `StaffRole` type). `Extract<UserRole, "admin" | "editor">` does the inverse — it keeps only the members that overlap, which is handy when narrowing a wide union down to the subset you actually handle in one branch of code.

## Code Walkthrough

See [code.ts](code.ts) for the full runnable file. Here is what each derived type does and why it exists.

**`User` (source of truth).** The one real interface. Every DTO below is computed from it, never hand-duplicated.

**`UpdateUserDto = Partial<Omit<User, "id">>`.** The PATCH body: every field optional, and `id` excluded because you never update a record's identity through its own update payload.

**`SafeUser = Omit<User, "passwordHash">`.** The API-response shape. Constructed as "everything except the one field that must never leave the server."

**`PublicProfile = Pick<User, "id" | "name" | "email">`.** The public-facing view: an explicit allow-list, appropriate when you want to be conservative by default and only expose fields you name.

**`FrozenUser = Readonly<User>`.** Used for a cached/shared instance that downstream code must not mutate in place.

**`rolePermissions: Record<UserRole, Permission[]>`.** A permissions table where the key type is a literal union, so the compiler enforces that every role has an entry.

**`createUser` factory plus `ReturnType<typeof createUser>` and `Parameters<typeof createUser>`.** Demonstrates pulling a type *out of* a function's signature instead of writing it independently.

**`StaffRole = Exclude<UserRole, "viewer">`.** A quick example of filtering a union without retyping its members.

## Advantages

- **Single source of truth.** Every derived shape recomputes from the base type; there is nothing to remember to keep in sync.
- **Safety by construction.** `Omit`-based "safe" types cannot accidentally leak a field you excluded; `Record` with a literal-union key forces exhaustive coverage.
- **Self-documenting intent.** `Partial<User>` reads as "a partial user," which is more informative than an unrelated interface name.
- **Less code to write and review.** No second interface to author, name, and keep updated.
- **Composable.** Utility types combine freely — `Partial<Omit<User, "id">>`, `Readonly<Pick<User, "id" | "role">>`, and so on.
- **Teaches the underlying mechanism.** Learning them is a gateway to writing your own mapped/conditional types when the built-ins do not cover a case.

## Disadvantages

- **Type-only, not a runtime guarantee.** `Omit<User, "passwordHash">` is a compile-time contract; if you build the object with a spread that still includes `passwordHash`, TypeScript will not delete it from the actual runtime object — the property is merely invisible to the type checker unless you also strip it at runtime.
- **Deep structures need extra work.** All of these operate one level deep; nested objects inside `T` are not made partial/readonly/etc. recursively unless you write (or import) a `DeepPartial`/`DeepReadonly` variant.
- **Over-nesting hurts readability.** `Partial<Pick<Omit<User, "passwordHash">, "name" | "email">>` is correct but hard to read; give heavily-composed types their own named alias.
- **`Record` with a non-literal key type loses the exhaustiveness benefit.** `Record<string, Permission[]>` does not force any particular keys to exist — the safety comes specifically from using a literal union as `K`.

## Common Mistakes

- **Hand-writing a "safe" or "update" type instead of deriving it.** *Why it happens:* it feels quicker in the moment. *Avoid:* always ask "is this just my base type with something added, removed, or changed?" before writing a new interface from scratch.

- **Forgetting utility types are shallow.** Beginners expect `Readonly<User>` to also freeze a nested `address` object. *Why:* the name suggests "fully immutable." *Avoid:* know that `Partial`/`Required`/`Readonly` only touch the top-level properties; reach for a deep variant when you truly need recursion.

- **Using `Omit` to remove a field but continuing to build the object with `{ ...user }`.** The type says the field is gone, but a runtime spread still copies it. *Avoid:* if the field must not exist at runtime (e.g. a password hash going out over HTTP), delete it explicitly or construct the object field-by-field.

- **Using `Record<string, V>` when you meant a specific literal union.** This silently gives up the "every key must be handled" guarantee. *Avoid:* key your `Record` with the literal union type whenever the set of keys is closed and known.

- **Confusing `Pick`/`Omit` with `Exclude`/`Extract`.** `Pick`/`Omit` operate on **object property keys**; `Exclude`/`Extract` operate on **members of a union type**. *Avoid:* ask "am I selecting properties of an object, or filtering members of a union?"

## When To Use

- Deriving request/response DTOs (create, update, public-view, safe-response) from one canonical entity type.
- Building compile-time-checked lookup tables keyed by a closed set of literals (roles, statuses, event names).
- Reusing a function's return or parameter types instead of re-declaring them (`ReturnType`, `Parameters`), especially for factories, third-party functions, or anything whose exact shape you do not want to hand-maintain.
- Narrowing or trimming union types (`Exclude`, `Extract`) when working with discriminated unions or literal-union enums.
- Freezing a shape you never want mutated after construction (`Readonly`).

## When NOT To Use

- **When the derived shape is not actually structurally related to any existing type.** Forcing an unrelated interface through `Pick`/`Omit` just to avoid writing a new interface adds confusion, not value.
- **When you need deep/recursive transformation** and the shallow built-in would silently under-deliver — write or import a deep variant (`DeepPartial`, `DeepReadonly`) instead, and say so explicitly.
- **When you need actual runtime immutability or stripping**, not just a compile-time type. `Readonly<T>` and `Omit<T, K>` do not touch the object at runtime — use `Object.freeze()` or explicit field deletion/allow-listing for that.
- **When a hand-written interface would genuinely be clearer**, e.g. a type that only coincidentally shares a couple of field names with another type but represents a conceptually different thing.

## Real Production Examples

- **NestJS / class-validator DTOs:** `PartialType(CreateUserDto)` from `@nestjs/mapped-types` is a runtime-aware wrapper built directly on the `Partial<T>` concept, generating update DTOs from create DTOs.
- **Redux Toolkit / React:** component prop types frequently use `Pick`/`Omit` to derive a narrower prop type from a shared domain type instead of redeclaring fields.
- **Express/NestJS response serializers:** `Omit<User, "passwordHash" | "internalNotes">` style types are extremely common for "the shape that is safe to send over the wire."
- **Prisma-generated types:** Prisma's client type helpers (`Prisma.UserGetPayload`, `Prisma.UserCreateInput`, etc.) are themselves built using the same mapped/conditional-type machinery, layered on top of the base model type.
- **Type-level routing/config tables:** `Record<RouteName, Handler>` and `Record<EventName, Payload>` patterns are standard in typed event-emitters and routers, giving compile-time exhaustiveness checks.
- **Function-type reuse:** libraries wrapping other libraries' functions (loggers, retries, memoizers) commonly use `Parameters<T>` and `ReturnType<T>` so the wrapper's signature always matches the wrapped function's, even if that function's types change.

## Where I Can Use This

Five realistic ideas for your own projects:

1. **CRUD DTOs.** For every entity, derive `Create*Dto` (often `Omit<Entity, "id" | "createdAt">`), `Update*Dto` (`Partial<Omit<Entity, "id">>`), and `Safe*` response types (`Omit<Entity, "passwordHash" | ...>`) instead of hand-writing each.
2. **Role/permission tables.** A `Record<Role, Permission[]>` (or `Record<Role, boolean>` per permission) that fails to compile if you add a new role and forget to give it permissions.
3. **Typed API clients.** Wrap a fetch/axios call with `ReturnType<typeof endpointFn>` so your client's return type always matches the endpoint function, even as it evolves.
4. **Config objects.** `Readonly<Config>` for a loaded configuration object that should never be mutated after startup.
5. **Generic wrapper utilities.** A `withLogging(fn)` or `withRetry(fn)` helper typed using `Parameters<F>` and `ReturnType<F>` so it works for *any* function without you re-declaring its signature.

## Similar Topics

- **Mapped types:** the underlying mechanism (`{ [K in keyof T]: ... }`) that `Partial`, `Required`, `Readonly`, `Pick`, and `Record` are built from.
- **Conditional types:** the underlying mechanism (`T extends U ? X : Y`) that `Exclude`, `Extract`, `ReturnType`, and `Parameters` are built from (the latter two also use `infer`).
- **Generics:** utility types are themselves generic types (`Partial<T>`); understanding generics is a prerequisite.
- **Template literal types:** a related, newer mapped-type feature for transforming string literal types (not covered here, but built on the same "transform the source type" philosophy).
- **Discriminated unions:** a common target for `Exclude`/`Extract` when narrowing a union of tagged object types.

## Summary

- Utility types **derive** a new type from an existing one instead of hand-duplicating it, keeping the two structurally linked.
- `Partial`, `Required`, and `Readonly` change **modifiers** (`?`, `readonly`) on every property.
- `Pick` and `Omit` change **which properties exist**, by allow-list or by deny-list respectively.
- `Record` builds a new object type from a key set and a value type, enforcing exhaustiveness when the key is a literal union.
- `ReturnType` and `Parameters` extract types **out of a function's signature** instead of re-declaring them.
- `Exclude`/`Extract` filter members of a **union type**, not properties of an object.
- Every one of these is implemented with ordinary **mapped types** and **conditional types** — there is no special compiler magic, and you can write your own when the built-ins do not fit.

## Key Takeaways

1. Utility types exist to derive shapes from one source of truth instead of hand-duplicating interfaces that then drift.
2. `Partial<T>` / `Required<T>` / `Readonly<T>` change property modifiers; they do not add or remove properties.
3. `Pick<T, K>` / `Omit<T, K>` change which properties exist, via allow-list or deny-list.
4. `Record<K, V>` shines when `K` is a literal union — you get compile-time exhaustiveness for free.
5. `ReturnType<T>` / `Parameters<T>` pull types out of a function signature instead of retyping it.
6. `Exclude<T, U>` / `Extract<T, U>` filter union members, which is a different axis from `Pick`/`Omit`'s object-key filtering.
7. All of these are built from mapped types (`{ [K in keyof T]: ... }`) and conditional types (`T extends U ? X : Y`) — learn those two mechanisms and you can build your own utility types.
8. These are shallow/one-level-deep by default; nested structures need a deep variant.
9. They are compile-time only — `Omit`/`Readonly` do not change anything about the object at runtime.
10. When no built-in fits, write your own mapped or conditional type; that is exactly how the built-ins themselves are made.

---

## Further Reading

**Official Documentation**
- TypeScript Handbook — Utility Types — https://www.typescriptlang.org/docs/handbook/utility-types.html
- TypeScript Handbook — Mapped Types — https://www.typescriptlang.org/docs/handbook/2/mapped-types.html
- TypeScript Handbook — Conditional Types — https://www.typescriptlang.org/docs/handbook/2/conditional-types.html
- TypeScript source — `lib.es5.d.ts` (where `Partial`, `Pick`, `Omit`, etc. are actually defined) — https://github.com/microsoft/TypeScript/blob/main/src/lib/es5.d.ts

**Blog Articles**
- Marius Schulz — "The Partial, Required, Readonly, and Record Types in TypeScript" — https://mariusschulz.com/blog/the-partial-type-in-typescript
- Total TypeScript — "Mapped Types" — https://www.totaltypescript.com/

**Open Source Projects**
- `@nestjs/mapped-types` (`PartialType`, `PickType`, `OmitType` for DTOs) — https://github.com/nestjs/mapped-types
- `type-fest` (a library of additional utility types built the same way, e.g. `PartialDeep`, `SetOptional`) — https://github.com/sindresorhus/type-fest
