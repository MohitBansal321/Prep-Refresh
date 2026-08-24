# ES6+ Features — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core language syntax (ES6/2015 and a few later additions). |
| **Template Literals** | Backticks + `${expr}` interpolation, real multi-line strings. Coerces via `toString()`: `` `${null}` `` → `"null"`, `` `${undefined}` `` → `"undefined"`. Tagged templates power styled-components/GraphQL tags. |
| **Destructuring** | Pull fields/items by pattern: `{ a } = obj`, `[x] = arr`. Nesting mirrors data shape · rename with `old: new` · defaults fire **only on `undefined`** (not `null`, not `0`) · combine: `{ role: r = "guest" }` · best in function params (options object done right). |
| **Spread** | `...` expands iterables/objects at literals & calls. Merge precedence = later keys win (`{ ...defaults, ...overrides }`). Array copy: `[...arr]`; object copy: `{ ...obj }` — **one level deep only**. |
| **Rest** | `function f(a, ...rest)` collects trailing args into a REAL array (unlike `arguments`). Must be last, only one per signature. In destructuring grabs leftovers: `[h, ...t]`, `{ profile, ...meta }`. |
| **Map/Set** | `Set`: uniqueness + O(1)-avg `has()`; dedupe one-liner `[...new Set(arr)]`; insertion order. `Map`: any key type (object identity!), O(1) `.size`, iterable directly. Fixed record → object; dynamic registry → Map. WeakMap/WeakRef: entries GC-able when the KEY dies — memory-safe metadata, not iterable. |
| **for...of vs for...in** | `of` = **values** of iterables (arrays, strings, Map/Set); `in` = enumerable **string keys** incl. extras added to arrays. Objects have no iterator → `for (const [k, v] of Object.entries(obj))`. |
| **Shallow Copy Trap** | Spread copies shape, not nested values: inner objects stay SHARED — mutating `copy.profile.city` leaks to the original. Deep copy plain data with `structuredClone()` or a library. |
| **Modern Operators** | `obj?.a?.b` safe access · `a ?? b` fallback for null/**undefined only** (`\|\|` falls through on 0/""/false) · `??=` `\|\|=` `&&=` conditional assignment · `1_000_000` numeric separators · `structuredClone(x)` deep copy. |
| **Modules** | `export const f = ...` / `export default X`; import named vs default vs namespace (`import * as ns`). Exports are live bindings; modules evaluate once per path → module-level singletons. |
| **Related Topics** | `let`/`const` + TDZ → js/scope-and-closures · arrows & lexical `this` → js/scope-and-closures · `?.`/`??` in depth → js/null-and-undefined · shallow-vs-deep by hand → js/immutability |

### Skeleton
```js
// destructure + defaults + rest, all at once
function handle({ user: { name = "anon", roles = [] } = {}, page = 1 }, ...rest) {}
const cfg   = { ...defaults, retries: 5 };          // spread merge, later wins
const uniq  = [...new Set(ids)];                     // dedupe
const deep  = structuredClone(original);            // real deep copy
let [head, ...tail] = list;
[a, b] = [b, a];                                     // swap
```

### Remember In One Sentence
> **ES6 syntax is sugar with sharp edges: destructuring/spread/rest compress code beautifully, but spread copies are shallow, defaults fire only on `undefined`, and `for...in` walks keys where you meant values.**

### Two Facts People Get Wrong
- **"{ ...obj } is a deep copy"? No** — it's shallow: nested objects/arrays remain shared references; use `structuredClone` for depth.
- **"`||` is fine for defaults"? No** — it clobbers legitimate falsy values (`0`, `""`, `false`); use `??` / default params, which respond only to `undefined`.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud, *then* check against the README.

1. What exactly does `const` freeze — the binding or the value? Show both an allowed and a forbidden mutation.
2. In destructuring, which three moves can you combine in one pattern? Write `{ role: r = "guest" }`'s effect in words.
3. When does a destructuring default kick in — for `undefined`, `null`, `0`, or all three? Why does that distinction matter for API data?
4. Explain the shallow-copy trap precisely: what IS copied by `{ ...original }` and what is shared? Name the built-in that fixes it.
5. Rest parameter vs `arguments` — give two concrete differences.
6. In object spread merging, which layer's keys win? How do you write a "defaults + overrides" merge?
7. Give two reasons to prefer `Map` over a plain object, and the rule of thumb for choosing between them.
8. State the `for...of` vs `for...in` difference in one sentence each, and what happens if you run `for...of` on a plain object.
