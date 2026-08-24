# `this` and Prototypes

JavaScript has no classical inheritance — it has **prototypes**. And its `this` keyword is not assigned where you *write* a function, but where you **call** it (with one famous exception: arrow functions). Almost every confusing JavaScript bug an interviewer probes — "why did my callback lose `this`?", "what's the difference between `__proto__` and `.prototype`?" — comes from these two mechanisms.

This module is split into two halves that belong together:

- **Part A — `this`:** the 5 binding rules, ranked by precedence, and the classic bugs they cause.
- **Part B — Prototypes:** how property lookup actually works, what `class` really is, and how `instanceof` decides.

> **Term: Binding.** The invisible connection between the word `this` inside a function body and the actual object it refers to at runtime. It is decided fresh **on every call** — the same function can have a different `this` each time it runs.

---

## Part A — `this`

### The core idea

`this` is a parameter that JavaScript fills in for you. You never assign to it; the **call site** does. So the question to train yourself to ask is never *"what is `this` in this function?"* but rather ***"how is this function being called right now?"***

There are exactly five rules. Learn them as a priority list — when two rules could apply, the higher-ranked one wins.

| Rank | Rule | Trigger at call site | `this` becomes |
|------|------|----------------------|----------------|
| 1 | `new` binding | `new Fn()` | the freshly created object |
| 2 | Explicit binding | `fn.call/apply/bind(...)` | the object you pass |
| 3 | Implicit binding | `obj.fn()` | `obj` (the thing before the dot) |
| 4 | Default binding | plain call `fn()` | `undefined` (strict) / `globalThis` (sloppy) |
| 5 | Lexical binding | arrow function | the `this` of the enclosing scope |

Arrows are listed last not because they lose — they are simply **not in the game**: an arrow ignores rules 1–4 entirely and inherits `this` lexically.

### Rule 1 — Default binding

A function called with nothing before it ("just `fn()`") gets default binding.

```js
function whereAmI() {
  return this;
}

whereAmI(); // sloppy mode: globalThis (the global object)
```

```js
'use strict';
function whereAmIStrict() {
  return this;
}
whereAmIStrict(); // strict mode: undefined
```

> **Term: Sloppy vs strict mode.** Non-strict ("sloppy") code predates ES5; in it, a receiver-less call falls back to the **global object** (`globalThis`), which historically caused accidental global mutations. Strict mode (`'use strict'`, or any ES module / `class` body, which are strict by default) makes `this` **`undefined`** instead — a safer, louder failure.

The interview-worthy detail: default binding is why calling an object's method through an extracted reference gives you `globalThis` or `undefined` instead of the object (see [Loss of `this`](#loss-of-this-the-two-classic-bugs)).

### Rule 2 — Implicit (object) binding

When the call site has a **receiver** — something before the dot — `this` is that receiver:

```js
const teacher = {
  name: 'Implicit',
  introduce() {
    return `Hi from ${this.name}`;
  },
};

teacher.introduce(); // "Hi from Implicit" — the dot binds `this` to teacher
```

Only the **last** dot counts, and only for the immediate call:

```js
const outer = {
  name: 'outer',
  inner: { name: 'inner', say() { return this.name; } },
};
outer.inner.say(); // "inner" — closest object wins
```

Two traps:

```js
// Trap 1: assignment breaks the chain
const fn = outer.inner.say;
fn(); // default binding! "undefined"

// Trap 2: passing the method as a callback detaches it
setTimeout(outer.inner.say, 0); // `this` is not outer.inner
```

Both are the same underlying mistake: implicit binding happens **at call time**, and both examples moved the function away from its dot.

### Rule 3 — Explicit binding: `call`, `apply`, `bind`

You can force `this` by hand:

```js
function greet(greeting, punctuation) {
  return `${greeting}, ${this.name}${punctuation}`;
}
const cat = { name: 'Cat' };
const dog = { name: 'Dog' };

greet.call(cat, 'Hello', '!');        // "Hello, Cat!"   args passed one by one
greet.apply(dog, ['Yo', '?']);        // "Yo, Dog?"      args passed as array
const boundGreet = greet.bind(cat);   // returns a NEW function, permanently bound
boundGreet('Hey', '.');               // "Hey, Cat."
```

Differences worth memorizing:

- `call(fn, ctx, a, b)` vs `apply(fn, ctx, [a, b])` — same behavior, different argument shape.
- `bind(ctx)` **returns a new function** whose `this` is locked. Re-binding a bound function changes nothing — the first bind wins.
- Passing primitives as the context wraps them (`true`, numbers) into objects in sloppy mode; `null`/`undefined` fall back to default binding.

> **Term: Hard binding.** `bind` produces a function whose `this` cannot be overridden later — not by `call`, not even by being used with a new receiver. That permanence is why it is the standard fix for callbacks.

### Rule 4 — `new` binding

When a constructor-style function is invoked with `new`, JavaScript performs four steps behind the scenes:

```js
function Robot(name) {
  // 1. a fresh object {} is created
  // 2. its [[Prototype]] is linked to Robot.prototype
  // 3. the function runs with `this` = that fresh object
  this.name = name;              // so `this` here IS the new robot
  // 4. unless we explicitly return an object, `this` is returned automatically
}
const bot = new Robot('New-Binding');
bot.name; // "New-Binding"
```

`new` outranks everything except arrows: even a hard-bound function called with `new` gets a brand-new object as `this`, ignoring the bound context. Conversely — a classic bug — calling a constructor **without** `new` in sloppy mode dumps its assignments onto `globalThis`.

### Rule 5 — Lexical binding: arrow functions

Arrow functions have **no `this` of their own**. `this` inside an arrow is whatever it was in the enclosing scope when the arrow was defined — like a normal variable lookup.

```js
const counter = {
  name: 'Lexical-Outer',
  run() {
    const arrow = () => `${this.name}`;      // inherits `this` from run()
    function regular() {
      return this && this.name;              // own binding per the call site
    }
    return [arrow(), regular.call(counter)];
  },
};
counter.run()[0]; // "Lexical-Outer" — arrow picked up the enclosing `this`
```

Because there is no `this` to redirect, arrows ignore `.call/.apply/.bind` for `this` purposes (those calls still forward arguments) and `new` cannot even be applied to them (it throws).

Practical rule of thumb: use regular functions for methods (you want dynamic `this`) and arrows for callbacks nested inside them (you want to keep it). Modern class syntax often removes the need entirely via field-arrow properties.

### Loss of `this` — the two classic bugs

These two patterns account for most real-world `this` bugs:

**Bug 1 — extracted method.**

```js
const user = {
  name: 'Ada',
  getName() { return this.name; },
};

const ref = user.getName;   // the function itself, detached from the dot
ref();                      // undefined / throws in strict — default binding!
```

Reading `user.getName` copies the *function reference*; the association with `user` existed only in the call syntax `user.getName()`.

**Bug 2 — callback.**

```js
document.addEventListener('click', user.getName);
// or:
setTimeout(user.getName, 1000);
// or (very common):
[1].map(user.getName);
```

Passing `user.getName` hands over the bare function. Whoever calls it later calls it without your receiver, so default binding applies. This is exactly why event-handler methods and React-era class components were littered with `.bind(this)` in constructors.

**The fixes**, in order of preference today:

```js
// 1. Arrow wrapper at the call site — creates the closure, keeps the receiver
setTimeout(() => user.getName(), 1000);

// 2. Hard bind once
const getNameBound = user.getName.bind(user);

// 3. Class-field arrow (classes): the method is bound per instance at construction
class Button {
  onClick = () => console.log(this.label);
  constructor(label) { this.label = label; }
}
```

### Classes and `this`

Class bodies are strict mode by default, so a detached class method gets `this === undefined` — it fails loudly rather than silently touching globals. Otherwise classes follow the same five rules. Two class-specific notes:

- Methods live on `Class.prototype`; instances do not carry copies (Part B explains why this matters).
- Handlers passed to DOM timers/listeners need binding exactly as above — hence the idiom `this.handleClick = this.handleClick.bind(this)` in older React, superseded by field-arrow methods.

---

## Part B — Prototypes

### Three words people conflate

> **Term: `[[Prototype]]`.** The internal, hidden link every object carries pointing at another object (or `null`). Property lookup follows this link. It is the *actual mechanism* of inheritance in JavaScript.

> **Term: `__proto__`.** A legacy getter/setter property that exposes `[[Prototype]]`. It works everywhere but is officially deprecated — use `Object.getPrototypeOf(obj)` / `Object.setPrototypeOf(obj, proto)` instead.

> **Term: `.prototype`.** A completely different thing despite the name: a property **of functions** (especially constructors and classes). When you call `new Fn()`, the created object's `[[Prototype]]` is set to `Fn.prototype`. Functions *have* a `.prototype`; ordinary objects don't.

The one-line disentangling:

```js
function Foo() {}
const f = new Foo();

f.__proto__ === Foo.prototype;                    // true — same link, two views
Object.getPrototypeOf(f) === Foo.prototype;       // true — the non-deprecated view
Foo.__proto__ === Function.prototype;             // functions are objects too!
```

Mnemonic: **`.prototype` is what a constructor *hands out*; `[[Prototype]]`/`__proto__` is what an object *holds*.**

### Prototype chain lookup

Reading a property walks the chain until found:

```js
const animal = {
  eats: true,
  speak() { return '...'; },
};
const rabbit = Object.create(animal); // rabbit.[[Prototype]] = animal
rabbit.jumps = true;

rabbit.jumps; // true     — own property
rabbit.eats;  // true     — found on `animal`, one link up
rabbit.missing; // undefined — whole chain exhausted
```

Mechanically: check the object → not found? follow `[[Prototype]]` → repeat until an object whose prototype is `null`. Assignment behaves differently: `rabbit.eats = false` does **not** walk the chain to update `animal`; it writes directly onto `rabbit`.

### Shadowing

An own property **shadows** (hides) a same-named property anywhere up the chain:

```js
const base = { level: 0, greet() { return `base ${this.level}`; } };
const mid = Object.create(base);
mid.level = 1;
const leaf = Object.create(mid);
leaf.level = 2;

leaf.level;          // 2 — own property wins, lookup stops immediately
delete leaf.level;
leaf.level;          // 1 — now mid's copy shows through
leaf.greet();        // "base 1" — note `this.level` resolves against leaf's chain
```

Shadowing is why instance data (`this.name = ...` in a constructor) can safely coexist with shared methods on the prototype: every instance shadows nothing until it assigns, and reads of methods flow up to the shared object.

### Constructor functions — pre-`class` inheritance

Before ES6 classes, objects were built by calling functions with `new` and hanging shared behavior on `.prototype`:

```js
function Animal(name) {
  this.name = name;                 // per-instance data
}
Animal.prototype.speak = function () {
  return `${this.name} makes a sound`;   // ONE shared function object
};

const a = new Animal('Rex');
const b = new Animal('Bella');

a.speak === b.speak;                // true — memory-efficient sharing
Object.keys(a);                     // ['name'] — the method was never copied
```

That last pair of lines is the entire point of prototypes: state lives per-instance; **behavior lives once**, shared through the chain.

Chaining constructors (the old "inheritance") took three deliberate steps — delegate calls, fix the `[[Prototype]]`, restore the constructor tag:

```js
function Dog(name) {
  Animal.call(this, name);          // reuse Animal's initialization
}
Dog.prototype = Object.create(Animal.prototype); // link the chains
Dog.prototype.constructor = Dog;                  // repair the .constructor pointer
Dog.prototype.bark = function () { return `${this.name}: woof`; };

const d = new Dog('Rex');
d.bark();            // "Rex: woof" — Dog.prototype
d.speak();           // "Rex makes a sound" — found on Animal.prototype, 2 links up
```

### `class` is sugar over prototypes

ES6 classes do not add a new inheritance mechanism — they formalize the above:

```js
class AnimalC {
  constructor(name) { this.name = name; }   // runs with `new`
  speak() { return `${this.name} says hi`; } // lands on AnimalC.prototype
}

AnimalC.prototype.speak;                     // function — same home as before
typeof AnimalC;                              // 'function' — still a function!
```

What the sugar adds: `Object.defineProperty`-style non-enumerable methods, mandatory `new` (calling a class without it throws — killing the global-pollution bug), strict-mode bodies, a real `super`, and `static` members. Underneath, lookup is the same prototype chain.

### `Object.create` — the direct API

`Object.create(proto)` builds an empty object whose `[[Prototype]]` is exactly `proto` — no constructor needed. It's the cleanest way to demonstrate the chain, build single-inheritance hierarchies from plain literals, and implement things like delegates and mixins.

### `instanceof` mechanics

`x instanceof C` answers one question: **does `C.prototype` appear anywhere in `x`'s prototype chain?**

```js
class Vehicle {}
class Car extends Vehicle {}
const c = new Car();

c instanceof Car;         // true — Car.prototype is a direct link
c instanceof Vehicle;     // true — reached via Car.prototype's chain
c instanceof Object;      // true — top of every default chain
```

It checks structure, not construction history: if you later rewire the chain with `Object.setPrototypeOf(c, {})`, then `c instanceof Car` becomes `false` even though `new Car()` built it. Symmetrically, `Object.create(C.prototype)` yields an object that passes `instanceof C` without ever invoking the constructor. And since primitives aren't objects, `'str' instanceof String` is `false` while `new String('str') instanceof String` is `true`.

### `hasOwnProperty` — separating own from inherited

```js
const child = Object.create({ inherited: 1 });
child.own = 2;

child.hasOwnProperty('own');        // true  — exists on the object itself
child.hasOwnProperty('inherited');  // false — came from the chain
'own' in child;                     // true  — `in` checks the WHOLE chain
```

Use it whenever you iterate an object and must exclude prototype baggage (`for...in` includes inherited enumerable keys; `Object.keys/values/entries` already restrict to own keys).

---

## How the two halves connect

Every rule from Part A plays out on the prototype machinery of Part B:

- Implicit binding (`obj.method()`) sets `this` to `obj` even when `method` lives several links up the chain — the *lookup* walks the chain, the *binding* uses the original receiver. That combination is what lets thousands of instances share one method that operates on their individual state.
- `new` creates the link to `Constructor.prototype` *and* supplies the fresh object as `this`.
- `instanceof` is pure chain-walking; shadowing determines whether an instance's own value hides a prototype default.

## Summary

- `this` is set **by the call site**, not the definition site — ask "how is this called?"
- Precedence: `new` > explicit (`call`/`apply`/`bind`) > implicit (`obj.fn()`) > default; arrows sit outside the system and inherit lexically.
- Detached references and callbacks lose implicit binding — fix with arrow wrappers, `.bind`, or field-arrow class properties.
- Objects hold `[[Prototype]]` (peeked at via `__proto__`, best read via `Object.getPrototypeOf`); functions carry `.prototype` which `new` installs as the new object's `[[Prototype]]`.
- Property reads walk the chain; writes create own properties; own values shadow inherited ones; `hasOwnProperty` tells them apart.
- Constructor-function inheritance and `class` produce the identical structure — classes are ergonomic sugar with extra safety (must use `new`, always strict).
- `instanceof` checks whether `C.prototype` lies somewhere in the object's chain — it's structural, not historical.

## Key Takeaways

1. Five rules, one question: *what does the call site look like?*
2. `obj.fn()` binds `this` to `obj` regardless of where `fn` was found in the chain.
3. Extracting a method or passing it as a callback drops the receiver — the #1 `this` bug.
4. Strict-mode default binding is `undefined`; sloppy-mode is `globalThis`.
5. Arrows have no `this`; `.bind` on an arrow is a no-op for context.
6. `__proto__` exposes the link; `.prototype` is a function property used by `new`.
7. Instances share one copy of prototype methods (`a.speak === b.speak`).
8. `class` = sugar: same prototypes underneath, stricter rules on top.
9. `instanceof` walks the chain looking for `C.prototype` — rewiring the chain changes the answer.
10. Prefer `Object.getPrototypeOf` / `Object.create`; treat `__proto__` as legacy.

---

## Further Reading

**Books**
- *You Don't Know JS: this & Object Prototypes* — Kyle Simpson (the deepest treatment of both halves of this module).
- *Eloquent JavaScript*, 3rd ed., Chapter 6 — Marijn Haverbeke (prototypes and classes, approachable).
- *JavaScript: The Good Parts* — Douglas Crockford (the classic critique of `this` and `new`).

**Official Documentation**
- MDN — `this`: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Operators/this
- MDN — Inheritance and the prototype chain: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Inheritance_and_the_prototype_chain
- MDN — `Object.create`: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object/create
- MDN — `Function.prototype.bind`: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_objects/Function/bind

**Blog Articles**
- "Understanding JavaScript Bindings" style deep dives — search Kyle Simpson's *this & Object Prototypes* summaries.
- MDN blog — Details of the object model (older but conceptually sharp on chains vs classes).
