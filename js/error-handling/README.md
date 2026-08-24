# Error Handling in JavaScript

## Why This Matters

In interviews, error handling separates people who have *written* JavaScript from
people who have *operated* it. Anyone can write a happy path; the questions that
come up again and again are:

- "What do you throw?" (Answer: an `Error` object, not a string.)
- "Does `finally` always run?" (Answer: yes — and that's also the trap.)
- "How do you catch an async error?" (Answer: `try/catch` around `await`, or `.catch()` — never both missing.)
- "Can a callback's error be caught by the caller?" (Answer: no — errors do **not** cross async boundaries.)

This module covers all of them, with runnable code in `code.js`.

---

## 1. You *Can* Throw Anything — But You Shouldn't

JavaScript's `throw` accepts any value:

```js
throw "oops";        // legal, but terrible
throw 42;            // legal, but terrible
throw new Error("oops"); // correct
```

Why is throwing primitives terrible? Because you lose everything useful:

```js
try {
  throw new Error("payment failed");
} catch (err) {
  console.log(err.message); // "payment failed"
  console.log(err.name);    // "Error"
  console.log(err.stack);   // full stack trace: where it was created AND thrown
}
```

An `Error` object gives you three fields:

| Field    | What it holds                                                        |
|----------|----------------------------------------------------------------------|
| `message` | Human-readable description of what went wrong.                       |
| `name`    | The error type name (`"TypeError"`, `"RangeError"`, your custom name...). |
| `stack`   | A string showing **where the error was created** (the call chain at construction time). |

The stack is the killer feature: `"oops"` as a string has no idea where it came
from. When your service fails at 3 a.m., the stack trace is the difference
between a two-minute fix and a two-hour investigation.

> **Interview one-liner:** *"Throwing a string works syntactically but destroys
> the stack trace and makes `instanceof Error` checks impossible downstream."*

---

## 2. Built-in Error Types

Use the built-in types to signal *what kind* of problem occurred:

```js
// TypeError — wrong type: calling non-functions, reading props of undefined/null
null.foo;
// TypeError: Cannot read properties of null (reading 'foo')

// ReferenceError — accessing a variable that was never declared
console.log(notDefinedAnywhere);
// ReferenceError: notDefinedAnywhere is not defined

// SyntaxError — the parser chokes on invalid code (thrown at parse time,
// so it cannot be caught by a try/catch in the same script)
eval("if (true {");
// SyntaxError: Unexpected token '{'

// RangeError — value out of valid range (wrong array length, bad number args)
new Array(-5);
// RangeError: Invalid array length
```

Others worth knowing: `URIError` (malformed `decodeURIComponent`),
`AggregateError` (groups multiple errors — see the validation pattern below),
and `EvalError` (mostly historical).

Rule of thumb: **throw `TypeError`/`RangeError` for programmer mistakes**
(garbage arguments), **throw plain `Error` or custom subclasses for domain
failures** (user not found, payment declined).

---

## 3. `try/catch/finally` Semantics

The shape everyone knows, with rules most people half-know:

```js
function risky() {
  try {
    throw new Error("boom");
  } catch (err) {
    console.log("caught:", err.message);
  } finally {
    console.log("finally runs");
  }
}
// caught: boom
// finally runs
```

The rules:

1. `catch` catches **synchronous** throws inside the `try` block only.
2. `finally` runs **no matter what**: normal completion, caught error,
   uncaught error, even `return`/`break`/`continue` from the block.
3. If `catch` itself throws, `finally` still runs before the new error propagates.
4. `catch` without binding (`catch { ... }`, ES2019) if you don't need `err`.
5. `finally` can exist without a `catch` — useful for cleanup where you want
   errors to propagate anyway.

### The return-in-finally trap

`return` in `finally` **overrides** every other exit path — including returns
from `try`, and even exceptions thrown from `try` or `catch`. It silently
swallows them:

```js
function trap() {
  try {
    throw new Error("important!");
  } catch (err) {
    return "from catch";
  } finally {
    return "from finally"; // overrides BOTH the return above AND the throw
  }
}
console.log(trap()); // "from finally" — the error vanished without a trace
```

Linters flag this for good reason. **Never put `return` (or `throw`) in
`finally`.** Use `finally` only for cleanup: closing files, releasing locks,
clearing timers.

---

## 4. Rethrowing and Error Wrapping

Two legitimate reasons to catch: **handle** it, or **add context then rethrow**.
Anything else is swallowing (see anti-patterns).

Rethrow when you can't handle it here:

```js
try {
  parseConfig();
} catch (err) {
  if (!(err instanceof SyntaxError)) throw err; // not mine to handle
  console.log("config file has bad syntax");
}
```

Wrap low-level errors into higher-level ones using the `cause` option (ES2022)
— this preserves the original stack instead of destroying it:

```js
class DatabaseError extends Error {} // shown properly in section 5

try {
  JSON.parse("{broken");
} catch (err) {
  const wrapped = new Error("Failed to load user config", { cause: err });
  console.log(wrapped.cause instanceof SyntaxError); // true
  throw wrapped;
}
```

Before `cause`, people chained manually (`new Error(msg + "; caused by " + err.message)`),
losing the inner stack. With `cause`, `err.cause` keeps the full original error:
you get context *at* the boundary *plus* the root cause for debugging.

---

## 5. Custom Error Subclasses

For application-level failures, define named error classes. This lets callers
react by **type**, not by string-matching messages:

```js
class AppError extends Error {
  constructor(message, options = {}) {
    super(message);
    this.name = "AppError";
    this.code = options.code ?? "APP_ERROR";
    this.retryable = options.retryable ?? false;
    if (options.cause) this.cause = options.cause;
    Error.captureStackTrace?.(this, AppError); // Node: hide constructor from stack
  }
}

class NotFoundError extends AppError {
  constructor(resource, id) {
    super(`${resource} ${id} not found`, { code: "NOT_FOUND" });
    this.name = "NotFoundError";
  }
}

try {
  throw new NotFoundError("user", 42);
} catch (err) {
  console.log(err instanceof NotFoundError); // true
  console.log(err instanceof AppError);      // true — subclass chain works
  console.log(err.code);                     // "NOT_FOUND"
}
```

Details that matter:

- Always `super(message)` first — otherwise `this` isn't initialized.
- Set `this.name`; otherwise every custom error prints as `"Error"` in logs.
- `err instanceof CustomError` works because of the prototype chain. Note:
  `instanceof` breaks across realms/iframes; for libraries, also check
  `err.name === "NotFoundError"` defensively.

---

## 6. Errors in Async Code

This is the #1 practical topic. Synchronous `try/catch` cannot see errors that
happen later on another tick. Async errors must be caught with async tools.

### try/catch around await

With `async/await`, rejected promises behave exactly like thrown exceptions —
a `try/catch` in the surrounding function catches them:

```js
async function load() {
  try {
    const data = await Promise.reject(new Error("network down"));
    return data;
  } catch (err) {
    console.log("caught:", err.message); // "caught: network down"
    return null;
  }
}
```

### .catch() on promises

If you're chaining raw promises, use `.catch()` — it's the promise-world
equivalent of `catch`:

```js
Promise.reject(new Error("timeout"))
  .then((v) => console.log("never reached"))
  .catch((err) => console.log("caught:", err.message)); // "caught: timeout"
```

Pick **one style per call site**. Mixing `await f().catch(...)` inside a
`try/catch` means whichever handles it first wins and the other never sees it.

### Unhandled rejections

A rejection nobody handles crashes modern Node (exit code 1):

```js
// DANGER (do not run): fires later, kills the process
// setTimeout(() => Promise.reject(new Error("late failure")), 10);

// SAFE: attach .catch immediately
setTimeout(() => {
  Promise.reject(new Error("late failure")).catch(
    (err) => console.log("handled:", err.message)
  );
}, 10);
```

For a last-resort net (not a strategy), Node offers
`process.on('unhandledRejection', handler)` and
`process.on('uncaughtException', handler)` — log there, then exit gracefully.
Using them to *keep running* hides corruption.

### Parallel work: Promise.all vs allSettled

`Promise.all` rejects as soon as the first promise rejects (fail-fast).
`Promise.allSettled` waits for all and reports each outcome — use it when you
want *every* result/error, e.g. batch jobs where partial success matters.

---

## 7. Errors Do NOT Cross Async Boundaries

This is the concept interviewers love because it trips up experienced devs:

```js
function oldStyle(cb) {
  setTimeout(() => {
    throw new Error("async boom"); // escapes ANY caller's try/catch
  }, 0);
}

try {
  oldStyle(() => {});
} catch (err) {
  console.log("never printed"); // unreachable
}
// Uncaught Error: async boom  ← crashes the process
```

Why? By the time the callback fires, the `try` block that called
`oldStyle()` has **already exited normally**. Each turn of the event loop gets a
fresh stack; a throw can only propagate up *its own* stack frame chain. The
caller's stack is long gone.

The fix — the Node convention: **pass the error to the callback** (error-first
callbacks):

```js
function fixedStyle(cb) {
  setTimeout(() => cb(null, "ok"), 0);
}
fixedStyle((err, result) => {
  if (err) return console.log("handled:", err.message);
  console.log(result);
});
```

Corollary: `EventEmitter` `'error'` events without a listener throw uncaught;
DOM/listener callbacks throwing can't be caught by the code that registered
them. Rule: **whoever starts the async operation must provide a channel
(callback arg, promise, event listener) for its future error — a `try/catch`
around the *start* call catches nothing that happens later.**

---

## 8. Fail-Fast Validation Pattern

Validate inputs at the boundary, throw immediately, keep internals clean:

```js
function transfer(from, to, amount) {
  if (typeof amount !== "number" || !Number.isFinite(amount)) {
    throw new TypeError(`amount must be a finite number, got ${amount}`);
  }
  if (amount <= 0) {
    throw new RangeError(`amount must be positive, got ${amount}`);
  }
  // ... core logic assumes valid inputs from here on
}
```

Fail fast beats "garbage in, weird behavior three functions later." The throw
site is the bug site. For *user-facing* validation (forms), the opposite style
is right: collect **all** problems and report them together via an
`AggregateError` — see exercise 3.

---

## 9. Common Anti-patterns

```js
// 1. Empty catch — the error existed, now it's gone forever
try { risky(); } catch {}

// 2. Swallow-and-return-default — hides real bugs behind fake success
try { return loadConfig(); } catch (err) { return DEFAULTS; }

// 3. Log-and-continue blindly — logged, but execution proceeds in broken state
try { chargeCard(order); } catch (err) {
  console.log("charge failed:", err.message);
}
sendConfirmationEmail(order); // sent even though payment failed!

// 4. Catching too broadly at the wrong level
try { everything(); } catch (err) { /* one giant net */ }

// 5. String-matching error messages
if (err.message.includes("timeout")) retry(); // brittle across library versions

// 6. return in finally — silently discards throws (section 3)
```

Each fix follows the same principle: **either handle the error meaningfully, or
let it propagate (possibly wrapped with context). Never make evidence
disappear.**

---

## Interview Quick Answers

- **What should you throw?** An `Error` instance (or subclass) — never strings/objects, for message/name/stack and `instanceof`.
- **Difference between `TypeError` and `ReferenceError`?** Wrong *type* used (prop of null, calling non-function) vs *undeclared variable* accessed.
- **When does `finally` NOT run?** Only on process exit / power loss. Even `return`/`throw` can't skip it — which is why `return` in `finally` is dangerous.
- **How do you catch an error inside a promise?** `.catch()`, or `await` it inside `try/catch`. There is no third option.
- **Can `try/catch` around a `setTimeout` catch the timer's error?** No — different stack. Handle inside the callback, or wrap the callback body.
- **What does `{ cause }` give you?** Chained errors: high-level context plus preserved root cause and its stack.

## Related Topics

- `js/immutability/` — defensive copying reduces the states errors can arise from.
- TypeScript modules — `unknown` in catch clauses forces narrowing before use.
- `sys-design/` — circuit breakers and retries build directly on error classification (`retryable` flags).

Next step: open `exercises.md` and build the reflexes yourself.
