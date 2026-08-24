# null & undefined — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | JS primitives; "absence of value" — two distinct kinds. |
| **undefined Sources** | Declared-not-initialized variable · function with no `return` · missing object property · missing argument · `void <expr>` (always). Produced **by the language**: "nothing assigned yet." |
| **null Meaning** | **You** assign it: intentional absence — cleared reference, or "searched, found none" (`querySelector`, DB lookups return `null`). |
| **typeof Quirk** | `typeof undefined` → `"undefined"` but `typeof null` → `"object"` (unfixable 1995 bug). Never trust `typeof x === "object"` alone; check `x !== null` first. |
| **Equality** | `null == undefined` → `true`; `null === undefined` → `false`. Idiom: `x == null` matches **only** those two (null equals nothing else, not even `0`/`""`/`false`). |
| **?? vs \|\|** | `\|\|` falls back on any **falsy**; `??` falls back only on **nullish** (`null`/`undefined`). `0 ?? 50 → 0`, `0 \|\| 50 → 50`. Can't mix `??` with `\|\|`/`&&` without parentheses. |
| **Optional Chaining** | `a?.b?.c` short-circuits the whole chain to `undefined` on first nullish link — no TypeError. Also guards calls (`fn?.()`) and indexes (`arr?.[0]`). |
| **Default Params Rule** | Defaults fire on **`undefined` only** (missing or explicit) — never on `null`, `0`, `""`, `NaN`. `f(null)` bypasses `function f(x = dflt)`. Normalize with `x = x ?? dflt`. |
| **Falsy Values** | Exactly six: `false, 0, "", null, undefined, NaN`. NOT falsy: `[]`, `{}`, `"0"`, `"false"` — all truthy! |
| **Gotchas** | Missing arg → `undefined` → `1 + undefined = NaN` · no-return function hands back `undefined` · reading deep props throws one line *after* the read · passing `null` hoping a default fires · `\|\|` eating valid `0`/`""`. |

### Skeleton
```js
// safe deep read + nullish fallback — the workhorse pattern:
const city = order?.customer?.address?.city ?? "unknown";

// is-empty guard (matches exactly null and undefined):
if (value == null) { /* handle both */ }

// correct defaulting when callers may send null:
host = host ?? "localhost";
```

### Remember In One Sentence
> **`undefined` is the system shrugging ("nothing assigned yet"), `null` is you answering "deliberately none" — so check emptiness with `== null` / `??`, not with truthiness.**

### Two Facts People Get Wrong
- `typeof null === "object"` — it really does; it's an unfixable historical bug.
- Default parameters trigger on `undefined` **only** — `f(null)` does NOT use the default.

---

## Recall Questions

1. List all four automatic sources of `undefined` plus the operator that always produces it.
2. State the convention distinguishing `undefined` from `null` in one sentence each. Who "produces" each?
3. What does `typeof null` return, why, and how do you write a correct "is an object" check anyway?
4. Why is `x == null` considered a safe idiom even though `==` is otherwise avoided? What exactly does it match?
5. Recite the six falsy values — then name three "look empty but are truthy" values.
6. Explain the difference between `||` and `??` using `volume = 0` as your example.
7. What happens when `?.` hits a property that exists but holds `null`, e.g. `obj.tag?.toUpperCase()` where `tag` is `null`?
8. For which argument values does a default parameter fire, and which caller input famously bypasses it? How do you normalize inside the body instead?
