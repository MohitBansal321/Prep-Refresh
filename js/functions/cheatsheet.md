# Functions — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | JS core language concept (functions as values). |
| **First-Class Functions** | Functions are values: store in variables, pass as args, return from functions, put in arrays/objects. Enables callbacks, HOFs, memoization. |
| **Declaration vs Expression** | **Declaration** (`function f() {}`) fully hoisted — callable before definition. **Expression** follows variable rules: `var` → `undefined`, `const`/`let` → TDZ until its line. Arrows are always expressions. |
| **Arrow vs Regular** | See table below — four differences, all from "arrows are lighter, not sugar." |
| **Parameters** | Defaults fire only on `undefined` (not `null`); defaults may reference earlier params. Rest (`...args`) = real array, must be last. Destructured options: `function api(path, { method = "GET" } = {})`. |
| **`arguments` object** | Array-*like* (has `.length`, no `.map`) in regular functions only; arrows inherit the enclosing function's `arguments` (bug source). Prefer rest params. |
| **Higher-Order** | Takes a fn and/or returns a fn. Built-ins: `map`, `filter`, `reduce`. Factories: `greeter = g => n => \`${g}, ${n}!\`` |
| **Pure Functions** | Same input → same output; zero side effects. Cacheable, testable, composable. Concentrate I/O at the edges. |
| **Composition** | `compose(f,g)(x) = f(g(x))`; `pipe(...fns)` runs left-to-right via reduce. Works because pieces are small + pure. |
| **Memoization** | Cache results keyed by args inside a closure; requires purity + benefits expensive pure fns (memoized fib: exponential → linear). |
| **Gotchas** | `(x) => ({})` needs parens to return an object · `connect(null)` skips nothing (defaults fire on `undefined` only) · arrow in a method reads outer `this` · rest must be last, one per signature. |
| **Use When** | Callbacks/pipelines → arrows · methods needing own `this`, constructors, generators → regular functions · repeated expensive pure calls → memoize · multi-step config → currying/factories. |
| **Related Topics** | Closures & scope · `this` binding (→ `js/this-and-prototypes/`) · call/apply/bind · event loop & async callbacks · recursion. |

### Arrow vs Regular — the four differences

| Aspect | Regular function | Arrow function |
|--------|------------------|----------------|
| Own `this`? | Yes — set by how it's called / bind/call/apply | No — lexical, inherited from enclosing scope |
| `arguments`? | Yes (array-like) | No — use `...rest` |
| Constructable with `new`? | Yes | No — throws TypeError |
| `prototype` property? | Yes | No |

### Skeleton
```js
function memoize(fn) {                 // HOF returning a closure
  const cache = new Map();             // private state, survives calls
  return (...args) => {
    const key = JSON.stringify(args);
    if (!cache.has(key)) cache.set(key, fn(...args));
    return cache.get(key);
  };
}
```

### Remember In One Sentence
> **Functions in JavaScript are ordinary values — so anything you can do with a value you can do with a function, and everything powerful about them (HOFs, closures, memoization) is just that fact applied twice.**

### Two Facts People Get Wrong
- Default parameters trigger on **`undefined` only** — pass `null` and the default does *not* kick in; you get `null`.
- An arrow's `this` is fixed at definition time (lexical), not at call time — `.bind()`, `.call()`, `.apply()` cannot change it.

---

## Recall Questions

1. What does it mean for functions to be "first-class"? Name three things it enables.
2. Which of the three definition forms is callable before its defining line, and why do the other two fail differently?
3. List all four differences between arrow and regular functions.
4. When exactly does a default parameter activate? Does `null` trigger it?
5. Why prefer rest parameters over the `arguments` object? Give two reasons.
6. Define a pure function precisely, and name two practical benefits of purity.
7. In `memoize(fn)`, what two things does the returned function close over, and why can't either be declared outside?
8. Write from memory: `const compose = ...` and `const pipe = ...` — which direction does each apply functions?
