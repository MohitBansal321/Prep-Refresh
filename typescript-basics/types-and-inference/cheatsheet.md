# Types and Inference — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | TypeScript type-system fundamental (language feature, not a design pattern). |
| **Intent** | Describe the exact shape and legal values of your data using the narrowest tool available, and let the compiler infer everything it safely can. |
| **Problem** | A `string`/`number` field can hold anything, a single object shape can't express "one of several different shapes," and duplicating shape definitions across files makes them drift apart. |
| **Solution** | Name object shapes with `interface`/`type`, restrict "one of a fixed set" with a literal-type union (or `enum`), combine shapes with `&`, offer a choice of shapes with `\|` plus a discriminant field, and trust inference for locals while annotating parameters and public return types. |
| **Pros** | Compile-time rejection of invalid values · one authoritative shape per concept · composable via `&`/`\|` · exhaustiveness checking with `never` · far less boilerplate thanks to inference. |
| **Cons / Gotchas** | `type` vs `interface` choice feels arbitrary until you hit a union or a merging need · `let` widens, `const` keeps literal types · unions of shapes need a discriminant or narrowing breaks · `const enum` isn't safe with every bundler · over-annotating adds noise, not safety. |
| **Use When** | Any status/priority/role-like field (literal union) · a public, extensible domain shape (`interface`) · combining a base shape with cross-cutting fields (`&`) · modeling events/results with genuinely different extra data per case (discriminated union). |
| **Avoid When / Common Mistakes** | Using `interface` for a union or primitive alias (illegal) · expecting `type` to merge like `interface` (compile error) · confusing `&` ("and") with `\|` ("or") · building a shape union with no discriminant field · annotating obvious locals "for clarity" · reaching for `const enum` without checking bundler support. |
| **Real Examples** | Express/Passport augmenting `Request` via declaration merging · Redux actions as discriminated unions · React prop types combining base props (`&`) with size/variant literal unions · GraphQL codegen emitting string-literal unions for enums · TypeScript's own `.d.ts` files merging interfaces. |
| **Related Topics** | Generics (parametrize a type) · Utility Types (`Partial`, `Pick`, `Omit`, `Record`) · Interfaces & Abstract Classes · Classes and Inheritance (`implements`) · Access Modifiers. |

### `type` vs `interface` — the real differences

| Capability | `type` | `interface` |
|---|---|---|
| Object shapes | Yes | Yes |
| Unions (`A \| B`) | Yes | No |
| Intersections (`A & B`) | Yes | Via `extends` (similar effect) |
| Primitive/tuple aliases (`type Id = string`) | Yes | No |
| Declaration merging (redeclare to add fields) | No — compile error | Yes |
| Implemented by a class (`implements`) | Yes | Yes |

### `enum` vs `const enum` vs string-literal union

| | `enum` | `const enum` | string-literal union |
|---|---|---|---|
| Runtime footprint | Real JS object emitted | None (inlined at usage) | None (erased entirely) |
| Reverse mapping (numeric enums) | Yes | No | No |
| Safe with single-file transpilers (esbuild/Babel/SWC) | Yes | Often unsafe/disallowed | Yes |
| Serializes cleanly to/from JSON | Awkward (extra object) | N/A (erased) | Trivial — it's just a string |
| `Enum.Member` namespacing | Yes | Yes | No — bare string literals |
| Default modern-TS recommendation | Sometimes | Rarely | Usually yes |

### Skeleton
```ts
type OrderStatus = "pending" | "paid" | "shipped" | "cancelled"; // type alias: names a union of literals

interface Order {                              // interface: object shape, mergeable
  id: string;
  status: OrderStatus;
}

interface Timestamped { createdAt: Date; updatedAt: Date; }
type AuditedOrder = Order & Timestamped;       // intersection: ALL fields of both

type ShippingPriority = "standard" | "express" | "overnight"; // literal union

enum ShippingPriorityEnum {                    // enum: real runtime object
  Standard = "STANDARD",
  Express = "EXPRESS",
}

// inference: no annotation needed, TS reads the initializer/context
const order = { id: "1", status: "pending" as OrderStatus }; // inferred shape
```

### Remember In One Sentence
> **A `type` alias can name anything — including unions and primitives — while an `interface` only names object shapes but can be reopened and merged; reach for `type` when you need a union or intersection, `interface` for extensible public shapes, and trust inference everywhere except function parameters and public return types.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Give one concrete case where `type` can do something `interface` cannot, and one case where `interface` can do something `type` cannot.
2. What happens if you declare the same `interface` twice in the same scope? Does the same thing work for `type`?
3. What is a discriminated union, and how does the discriminant field let TypeScript narrow inside a `switch`?
4. What does `Order & Timestamped` require of a value, compared to `Order | Timestamped`?
5. Why does `const maxRetries = 3` infer the literal type `3`, while `let retryCount = 0` infers the wider type `number`?
6. What is "contextual typing"? Give an example involving `Array.prototype.reduce`/`map`.
7. Name two places you should annotate a type explicitly even though TypeScript could infer it, and explain why for each.
8. Compare `enum`, `const enum`, and a string-literal union on runtime footprint and compatibility with single-file transpilers like esbuild/Babel.
9. Why do numeric enums support "reverse mapping" but string enums do not?
10. If you need to send a `status` field over JSON to an API, which representation (enum / const enum / string-literal union) serializes most naturally, and why?
