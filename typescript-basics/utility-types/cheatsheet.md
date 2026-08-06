# Built-in Utility Types — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | TypeScript type-level tooling (built on mapped types + conditional types). |
| **Intent** | Derive a new type from an existing one — optional, readonly, subset, remapped, or extracted from a function signature — instead of hand-duplicating an interface. |
| **Problem** | Multiple shapes of the same entity are needed (create DTO, update DTO, safe response, lookup table) and hand-writing each one lets them drift out of sync with the source type. |
| **Solution** | Compute the derived shape generically from the source type using a built-in utility type (`Partial`, `Pick`, `Omit`, `Record`, `ReturnType`, ...), so it updates automatically when the source changes. |
| **Pros** | Single source of truth · no manual sync · self-documenting intent · composable · reveals the underlying mapped/conditional-type mechanism so you can build your own. |
| **Cons/Gotchas** | Compile-time only, not enforced at runtime · shallow (one level deep) by default · deep nesting needs custom `DeepPartial`/`DeepReadonly` · heavy composition hurts readability · `Record<string, V>` loses exhaustiveness (use a literal union key instead). |
| **Use When** | Deriving create/update/response DTOs from one entity · building exhaustive literal-keyed lookup tables · reusing a function's return/parameter types · filtering literal-union types. |
| **Avoid When / Common Mistakes** | The "derived" type is not actually structurally related to the source · you need real runtime stripping/freezing, not just a compile-time type · confusing `Pick`/`Omit` (object keys) with `Exclude`/`Extract` (union members) · assuming `Readonly`/`Partial` are deep. |
| **Related Topics** | Mapped types (`{ [K in keyof T]: ... }`) · conditional types (`T extends U ? X : Y`) · generics · template literal types · discriminated unions · `type-fest` / `@nestjs/mapped-types`. |

### The Utility Types at a Glance

| Utility | What it changes | One-line use case |
|---------|------------------|--------------------|
| `Partial<T>` | Makes every property optional | PATCH/update DTO body |
| `Required<T>` | Makes every property mandatory | "fully hydrated with defaults" shape |
| `Readonly<T>` | Makes every property immutable | frozen cached entity |
| `Pick<T, K>` | Keeps only listed keys | public/allow-listed view |
| `Omit<T, K>` | Removes listed keys | safe API response (strip secrets) |
| `Record<K, V>` | Builds an object type from a key set | exhaustive role/permission lookup |
| `ReturnType<T>` | Extracts a function's return type | reuse a factory's output shape |
| `Parameters<T>` | Extracts a function's parameter tuple | typed wrapper/logger around `fn` |
| `Exclude<T, U>` | Removes members from a union | narrow a role/status union |
| `Extract<T, U>` | Keeps only overlapping union members | isolate a subset of a union |

### Skeleton

```ts
interface User {
  id: string;
  name: string;
  email: string;
  passwordHash: string;
  role: "admin" | "editor" | "viewer";
}

type UpdateUserDto = Partial<Omit<User, "id">>;        // PATCH body: everything optional except id is gone
type PublicProfile  = Pick<User, "id" | "name" | "email">; // allow-list
type SafeUser       = Omit<User, "passwordHash">;      // deny-list
type RolePermissionMap = Record<User["role"], string[]>;   // exhaustive lookup, keyed by literal union

function createUser(name: string, email: string, role: User["role"]): User { /* ... */ return {} as User; }
type NewUser       = ReturnType<typeof createUser>;    // = User, without redeclaring it
type CreateUserArgs = Parameters<typeof createUser>;   // = [string, string, User["role"]]
```

### Remember In One Sentence
> **Utility types are mechanical transformations of one source type — learn the handful of mapped/conditional types they're built from, and you can derive any shape you need without ever hand-duplicating an interface.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. What problem do utility types solve that hand-writing a second interface does not?
2. What is the one-sentence difference between `Pick<T, K>` and `Omit<T, K>`?
3. Write, from memory, the mapped-type definition of `Partial<T>` and of `Readonly<T>`.
4. Why does `Record<K, V>` give you a compile-time exhaustiveness guarantee only when `K` is a literal union?
5. What do `ReturnType<T>` and `Parameters<T>` extract, and what TypeScript feature (besides conditional types) do they both rely on internally?
6. `Exclude<T, U>` and `Extract<T, U>` operate on unions, not object properties — how is that different from `Pick`/`Omit`?
7. Are `Partial`, `Readonly`, and `Omit` deep or shallow by default? What do you reach for when you need deep behavior?
8. Do `Omit<User, "passwordHash">` or `Readonly<User>` change anything about the object at *runtime*? Why or why not?
9. Name the two generic-programming features (with definitions) that every built-in utility type is ultimately built from.
10. Give a realistic example each of `Partial<T>`, `Omit<T, K>`, and `Record<K, V>` from a `User`-shaped domain.
