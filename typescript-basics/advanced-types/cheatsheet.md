# Type Guards, Narrowing & Discriminated Unions — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core TypeScript type-system technique (control-flow analysis). |
| **Intent** | Prove to the compiler which member of a union type a value currently is, so type-specific fields/methods become safe to use. |
| **Problem** | A union type (`string \| number`, or a family like `Shape`) only lets you use what every member has in common — until you demonstrate, in a form TS recognizes, which member you actually hold. |
| **Solution** | Use a recognized narrowing check — `typeof`, `instanceof`, `in`, a custom `x is Foo` type predicate — or, for a family of related shapes, give every member a shared **literal** tag field (`kind`) and `switch` on it. |
| **Techniques** | **`typeof`** (primitives) · **`instanceof`** (class instances) · **`in`** (property existence) · **custom predicate** `x is Foo` (reusable/complex checks) · **discriminated union + switch** (whole object families). |
| **Exhaustiveness** | In the `switch`'s `default` case, assign the leftover value to a variable typed `never`. If a union member is ever added without a matching `case`, this line fails to compile. |
| **Pros** | Compile-time safety instead of runtime `TypeError`s · self-documenting narrowed branches · exhaustiveness makes unions safe to extend · reusable validation via predicates · zero added runtime cost from the type system itself. |
| **Cons / Gotchas** | Tag field must be a **literal** type, not `string`, or narrowing breaks · narrowing is lost across destructuring/reassignment/`await`/closures · a wrong custom predicate lies to the compiler silently · `instanceof` is useless on interfaces/object literals. |
| **Use When** | A parameter can legitimately be one of a few primitives or shapes · a family of related object shapes needs different per-case handling (API results, actions, AST nodes) · untrusted `unknown`/`any` data must be validated before use · the union is expected to grow. |
| **Avoid When / Common Mistakes** | Forcing a fake tag onto shapes with no natural discriminant · using `as Foo` instead of a real guard · writing a checker that returns `boolean` instead of `x is Foo` (it won't narrow) · omitting the `never` exhaustiveness check · needing rich external validation (prefer Zod/io-ts over hand-rolled predicates). |
| **Real Examples** | TypeScript's own AST (`ts.Node.kind: SyntaxKind`) · Redux/Redux Toolkit actions tagged by `type` · Express `req.query: string \| string[] \| undefined` · GraphQL codegen `__typename` unions · Node `fs.Dirent.isFile()/.isDirectory()` · Zod/io-ts runtime validators. |
| **Related Topics** | Union & intersection types · type assertions (`as`) vs type guards · the `never` type · control-flow analysis · exhaustiveness checking · schema validation libraries (Zod, io-ts) · algebraic data types / tagged unions in other languages. |

### The Five Techniques At A Glance

| Technique | Syntax | Narrows |
|-----------|--------|---------|
| `typeof` guard | `if (typeof x === "string")` | Primitives |
| `instanceof` guard | `if (x instanceof MyClass)` | Class instances |
| `in` guard | `if ("prop" in x)` | Object shapes by property presence |
| Custom predicate | `function isFoo(x: unknown): x is Foo` | Anything, via arbitrary logic |
| Discriminated union | `switch (x.kind) { case "a": ... }` | A whole family of tagged object shapes |

### Skeleton
```ts
// 1. Discriminated union: every member shares a literal-typed tag field.
interface Circle { kind: "circle"; radius: number; }
interface Square { kind: "square"; side: number; }
type Shape = Circle | Square;

// 2. switch on the tag narrows the WHOLE object per case, not just the tag.
function area(shape: Shape): number {
  switch (shape.kind) {
    case "circle":
      return Math.PI * shape.radius ** 2;   // shape: Circle here
    case "square":
      return shape.side ** 2;               // shape: Square here
    default: {
      // 3. Exhaustiveness check: fails to COMPILE if a new member is added
      //    to Shape without a matching case above.
      const _exhaustive: never = shape;
      throw new Error(`Unhandled shape: ${JSON.stringify(_exhaustive)}`);
    }
  }
}

// 4. Custom type predicate: validates `unknown` into a trusted type.
function isShape(x: unknown): x is Shape {
  return typeof x === "object" && x !== null && "kind" in x;
}
```

### Remember In One Sentence
> **Narrowing is how you prove to the compiler, in a language it recognizes, which member of a union you're holding — and the `never` exhaustiveness check is how the compiler promises to remind you if you ever forget one.**

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. Name the five narrowing techniques and give each a one-line description of what it narrows.
2. Why does the shared tag field on a discriminated union need to be a literal type instead of `string`?
3. What is the difference between a type guard and a type assertion (`as`)? Which one performs a runtime check?
4. Why won't a function that returns plain `boolean` narrow its argument, even if its logic is correct?
5. What does the `never` exhaustiveness check actually catch, and where do you put it?
6. Give two ways narrowing can be silently lost after you've already checked a value.
7. Why is `instanceof` useless against values typed only by an `interface`?
8. When would you reach for the `in` operator instead of a discriminated union?
9. Where should a custom type predicate typically be used in a real application (what kind of data)?
10. Give one real production example (a library or framework) that uses a discriminated union, and name its tag field.
