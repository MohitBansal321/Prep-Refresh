# `this` & Prototypes — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core JavaScript language mechanics — runtime context (`this`) and object linkage (prototype chain). |
| **this Determined By** | The **call site**, not the definition. Ranked: **1) `new`** → fresh object · **2) explicit** `call`/`apply`/`bind` → passed context (first `bind` wins) · **3) implicit** `obj.fn()` → the thing before the dot · **4) default** plain call → `undefined` (strict) / `globalThis` (sloppy). Arrows sit outside this list entirely. |
| **Arrow Functions** | No own `this` — inherit it **lexically** from the enclosing scope at definition. Ignore `call`/`apply`/`bind` for context; cannot be used with `new`. Use arrows for callbacks, regular functions for methods. |
| **Loss of this** | Two classic bugs: **extracted method** (`const f = obj.fn; f()`) and **method as callback** (`setTimeout(obj.fn, ...)`). Both detach the function from its dot, so default binding fires. |
| **Fixes** | 1) Arrow wrapper at the call site: `() => obj.fn()` · 2) Hard-bind once: `fn.bind(obj)` · 3) Class-field arrow method: `onClick = () => {...}` (bound per instance) · 4) Wrapper/call-time re-attach: `fn.call(ctx)`. |
| **Prototype Chain** | Every object has an internal `[[Prototype]]` link. Property **reads walk the chain** until found or `undefined`; **writes create own properties**; own values **shadow** inherited ones. State per instance, behavior shared via one prototype object (`a.speak === b.speak`). |
| **__proto__ vs prototype** | `[[Prototype]]`/`__proto__`: the link an **object holds** (use `Object.getPrototypeOf`) · `.prototype`: a property a **function carries**, which `new` installs as the new object's `[[Prototype]]`. `f.__proto__ === Fn.prototype` after `new Fn()`. |
| **Classes** | Sugar over prototypes: methods land on `Class.prototype`, instances share them, lookup is the same chain. Extras: mandatory `new`, strict-mode bodies, non-enumerable methods, real `super`, `static`. |
| **instanceof** | `x instanceof C` ⇔ `C.prototype` appears anywhere in `x`'s chain. **Structural, not historical** — rewiring the chain with `setPrototypeOf` changes the answer; `Object.create(C.prototype)` passes without running the constructor. |
| **Gotchas** | Strictness is inherited from surrounding code (a `'use strict'` file has no sloppy functions inside) · class methods detach to `undefined` (loud), sloppy methods to `globalThis` (silent corruption) · constructor called without `new` pollutes globals in sloppy mode · re-binding never overrides the first bind · `hasOwnProperty` filters own keys while `in` checks the whole chain · `for...in` walks inherited enumerables, `Object.keys` doesn't. |

### Skeleton

```js
// this rules
obj.fn();            // implicit: this = obj
const b = fn.bind(ctx); b();   // explicit/hard: this = ctx forever
new Fn();            // new: this = fresh object linked to Fn.prototype
fn();                // default: undefined (strict)
const arrow = () => this;      // lexical: inherits from enclosing scope

// prototypes
function Animal(name) { this.name = name; }     // state per instance
Animal.prototype.speak = function () {...};      // behavior shared ONCE

class AnimalC {                       // identical structure underneath
  constructor(name) { this.name = name; }
  speak() {...}                       // lives on AnimalC.prototype
}
```

### Remember In One Sentence

> **`this` is decided by how a function is *called* (`new` > explicit > implicit > default, with arrows inheriting lexically), and inheritance is just objects linked to other objects — property reads walk that chain while `class` merely formalizes the wiring.**

### Two Facts People Get Wrong

- "`__proto__` and `.prototype` are the same thing." **No** — `__proto__`/`[[Prototype]]` is the link an object *holds*; `.prototype` is the property a *function hands out* via `new`. They only meet in `f.__proto__ === Fn.prototype`.
- "Extracting a method keeps its connection to the object." **No** — the binding existed only in the call syntax `obj.fn()`; `const f = obj.fn; f()` falls back to default binding (`undefined` under strict mode).

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write it, *then* check against the README. If you miss one, that section is the only thing you need to re-study.

1. List the 5 binding rules in precedence order and name the exact call-site trigger for each.
2. Why does `setTimeout(obj.method, 1000)` lose `this`, and what are three different fixes?
3. What does default binding produce in strict vs sloppy mode, and why can't a sloppy function exist inside a `'use strict'` file?
4. How do arrow functions get their `this`, and why is `.bind` on an arrow pointless for context?
5. Explain the difference between `[[Prototype]]`, `__proto__`, and `Function.prototype` in two sentences.
6. Walk through what `new Fn()` does step by step — including which binding rule wins and where `Fn.prototype` ends up.
7. What are the three steps to wire up inheritance with constructor functions, and which one do people forget?
8. How exactly does `x instanceof C` decide true/false, and why does `Object.setPrototypeOf(x, {})` flip the answer afterwards?

