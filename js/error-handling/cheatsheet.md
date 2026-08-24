# Error Handling — One-Minute Cheatsheet

| Field | Summary |
|-------|---------|
| **Category** | Core JS language/runtime concept: exceptions, `try/catch/finally`, async error channels. |
| **What To Throw** | Always an `Error` instance (or subclass) — never strings/primitives. You get `.message`, `.name`, `.stack` and working `instanceof` checks. A thrown string has no idea where it came from. |
| **Built-in Types** | `TypeError` = wrong type used (prop of null, calling non-function) · `ReferenceError` = undeclared variable · `RangeError` = value out of range (bad array length, negative repeat) · `SyntaxError` = parse-time, uncatchable in the same script · plus `URIError`, `AggregateError`, `EvalError` (historical). |
| **try/catch/finally Rules** | `catch` sees only synchronous throws inside its `try` · `finally` runs NO MATTER WHAT (return/throw/break can't skip it) · if `catch` throws, `finally` still runs first · `catch {}` without binding (ES2019) · `finally` works alone for cleanup that lets errors propagate. |
| **Rethrow/Wrapping** | Catch only to HANDLE or to ADD CONTEXT then rethrow (`if (!(err instanceof X)) throw err;`). Wrap with `{ cause: err }` (ES2022) — high-level message at the boundary + preserved root cause and stack. |
| **Custom Errors** | `class AppError extends Error` — `super(message)` FIRST, set `this.name`, add domain fields (`code`, `retryable`). React by TYPE via `instanceof` (prototype chain), never string-matching messages. Note: `instanceof` breaks across realms; also check `err.name` defensively. |
| **Async Errors Rule** | `try/catch` around `await` catches rejections like throws; raw promises need `.catch()` — pick ONE style per call site. Unhandled rejections crash modern Node. Errors do NOT cross async boundaries: a throw inside a timer callback escapes any caller's `try/catch` — provide a channel (error-first callback / promise / event listener). |
| **Anti-patterns** | Empty catch · swallow-and-return-default · log-and-continue into broken state · one giant try around everything · string-matching `err.message` · `return` in `finally`. Fix rule: handle meaningfully or propagate (wrapped) — never make evidence disappear. |
| **Gotchas** | `return` in `finally` overrides returns AND silently swallows throws · `finally` skips only on process exit · stacks record CREATION time (construct at failure site) · `await f().catch(...)` inside a try/catch = first handler wins · fail-fast (`TypeError` at boundary) for programmers, aggregate-all (`AggregateError`) for user-facing forms. |
| **Related Topics** | `js/async-javascript` (rejections, combinators) · TS `unknown` in catch clauses · sys-design circuit breakers/retries built on `retryable` classification. |

### Skeleton

```js
class AppError extends Error {
  constructor(message, { code = "APP_ERROR", cause } = {}) {
    super(message);                    // first — initializes `this`
    this.name = "AppError";            // else logs print generic "Error"
    this.code = code;
    if (cause) this.cause = cause;     // preserve root cause + stack
  }
}
// wrap low-level → high-level:
try { JSON.parse(raw); }
catch (err) { throw new AppError("bad config", { code: "CONFIG", cause: err }); }
```

### Remember In One Sentence

> **Throw `Error` objects, use `finally` only for cleanup, catch async errors
> with async tools (`await` + `try/catch` or `.catch()`), and either handle
> an error meaningfully or rethrow it wrapped with context — never swallow it.**

### Two Facts People Get Wrong

- "`try/catch` around `setTimeout(fn)` catches what `fn` throws" — **No.**
  The caller's try block has already exited by the time the callback fires;
  each event-loop turn gets a fresh stack.
- "`return` in `finally` is harmless cleanup" — **No.** It overrides every
  other exit path, including active exceptions, silently discarding them.

---

## Recall Questions (answer from memory — no peeking)

Use these for active recall during revision. Say the answer out loud or write
it, *then* check against the README. If you miss one, that section is the only
thing you need to re-study.

1. What are the three fields every `Error` gives you, and what exactly does the stack record? Why is throwing a string terrible?
2. Distinguish `TypeError`, `ReferenceError`, and `RangeError` with one example each. Which do you reserve for programmer mistakes vs domain failures?
3. List the five `try/catch/finally` rules. When does `finally` genuinely NOT run?
4. Explain precisely how `return` in `finally` destroys evidence — what three things does it override?
5. What are the only two legitimate reasons to catch an error? Show how `{ cause }` improves on pre-2022 manual chaining.
6. Write the custom-error checklist from memory: what must come before setting fields on `this`, which field must you always set, and why might a library check `err.name` in addition to `instanceof`?
7. Give both ways to handle a rejected promise, name the third option that doesn't exist, and explain why mixing styles per call site is dangerous.
8. Why can't the caller of `setTimeout(fn)` catch a throw from inside `fn`? State the rule about who must provide the error channel.
