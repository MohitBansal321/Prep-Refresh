# Builder Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting the telescoping-constructor / bag-of-optional-args smell and moving construction into a fluent builder with a single validation gate.

> Rule of thumb for every exercise: put **all validation in `build()`**, never in the setters — cross-field invariants can only be checked once every field is present. Make the finished **product immutable** (`readonly` + `Object.freeze`). Add a **Director only when the same recipe repeats**.

---

## Easy — Pizza Order Builder

You have a `Pizza` product with these fields: `size` ("S" | "M" | "L", required), `crust` ("thin" | "thick", defaults to "thin"), and `toppings` (a list, optional, starts empty).

**Task:** Write a `PizzaBuilder` with a fluent interface — `setSize`, `setCrust`, `addTopping` (callable many times), and `build()`. Each step returns `this` so calls can be chained. `build()` must throw if `size` was never set. Prove that you can chain:

```ts
const p = new PizzaBuilder().setSize("L").addTopping("olives").addTopping("mushrooms").build();
```

**Acceptance:** The chained call returns a `Pizza` with size `"L"`, crust `"thin"` (the default), and two toppings. Calling `build()` without a size throws.

---

## Medium — SQL SELECT Query Builder

Your app assembles read queries. A raw string like `"SELECT " + cols + " FROM " + t + where + order + limit` is unreadable and easy to get wrong.

Target a `SqlQuery` product that exposes `toSql(): string`. Write a `SelectQueryBuilder` with:

```ts
interface SelectQueryBuilder {
  select(...columns: string[]): this;
  from(table: string): this;
  where(clause: string): this;      // callable multiple times → AND-ed together
  orderBy(column: string, dir?: "ASC" | "DESC"): this;
  limit(n: number): this;
  build(): SqlQuery;
}
```

**Task:** Implement it fluently. In `build()`, enforce these invariants **together**:
- `from` (table) is required.
- If `select` was never called, default to `*`.
- `limit`, if set, must be a positive integer.
- **Cross-field rule:** you may only `orderBy` a column that appears in the selected columns (unless the selection is `*`).

**Show:** the same builder produces two different queries in sequence — prove you `reset()` state between builds so the second query does not leak `where` clauses from the first.

**Bonus constraint:** Emit **parameterised** SQL (`WHERE age > ?`) plus a `params` array, not string-interpolated values, to make SQL injection structurally impossible.

---

## Hard — HTTP Request Builder with a Director (extend the README scenario)

Start from the `FluentHttpRequestBuilder` in [code.ts](code.ts). Extend it without breaking any existing behaviour.

**Task:**
1. Add a `setAuth(scheme: "Bearer" | "Basic", credentials: string)` step that sets the `Authorization` header, and a `setJson(body)` convenience that sets the body *and* forces `content-type: application/json`.
2. Add two new cross-field invariants to `build()`: (a) a `Basic` auth value must be base64-looking (non-empty, no spaces); (b) if `maxRetries > 0`, the method must be idempotent (`GET`, `PUT`, `DELETE`, `HEAD`) — reject retries on `POST`/`PATCH`.
3. Add a **second concrete builder**, `CurlCommandBuilder`, that implements the *same* `HttpRequestBuilder` interface but whose `build()` returns a `curl` command string instead of an `HttpRequest`. (You will need to generalise the return type — think about how.) This demonstrates "same construction process, different representation."
4. Add a Director recipe `jsonPatch(base, path, token, patch)` and prove the **same** `ApiRequestDirector` can drive either concrete builder.

**Think about:** where does the `content-type` convenience belong — setter or `build()`? Why can invariant (b) *not* live in `setRetries`?

---

## Real-World Challenge — Notification Builder for a NestJS Service

Build a `Notification` product and a `NotificationBuilder` used across a backend service. A notification has many optional fields and real cross-field rules.

```ts
interface NotificationBuilder {
  to(userId: string): this;                       // required
  channel(c: "email" | "sms" | "push"): this;     // required
  subject(s: string): this;                       // email only
  body(text: string): this;                       // required
  template(name: string): this;                   // optional; mutually exclusive with body
  withVar(key: string, value: string): this;      // template variables
  scheduleAt(when: Date): this;                   // optional; must be in the future
  build(): Notification;
}
```

**Requirements:**
- `build()` enforces, together: `to`, `channel`, and (exactly one of `body` / `template`) are present.
- **Cross-field invariants:** `subject` is only valid when `channel === "email"`; `sms` bodies must be ≤ 160 characters; `scheduleAt`, if present, must be in the future.
- Throw a dedicated `NotificationBuildError` describing the first violated invariant.
- Add a `NotificationDirector` with a reusable `welcomeEmail(userId, name)` recipe (sets channel email, the `"welcome"` template, and a `name` variable). Prove a caller can take the director's builder and add `.scheduleAt(...)` before `build()`.
- The finished `Notification` must be immutable.

**Stretch:** Write a unit test that asserts every validation branch throws (missing recipient, sms too long, subject on a push notification, past schedule time) and that the happy path produces a frozen object. Note how having *one* validation gate makes this test suite exhaustive and simple.

---

## Bonus Challenge — Builder vs Options Object, and Type-Safe Required Fields

1. **Head-to-head.** Take the `SelectQueryBuilder` from the Medium exercise and rewrite the *same* capability as a plain options-object constructor: `new SqlQuery({ table, columns, where, orderBy, limit })`. Then write a short comparison (in comments) of what the builder gives you that the options object does *not*: incremental/conditional construction (adding a `where` inside a loop), a single validation gate that makes an invalid `SqlQuery` unrepresentable, and a reusable recipe. Decide honestly which you would ship for *this* object.

2. **Compile-time required fields (the "staged builder" trick).** Redesign the Easy `PizzaBuilder` so that calling `build()` before `setSize` is a **compile error**, not a runtime throw. Hint: have `setSize` return a *different* interface type (`PizzaBuilderWithSize`) that is the only one exposing `build()`. Explain the tradeoff versus runtime validation.

3. **Study real query builders.** Open the public API docs of **Knex** (`knex('users').select('*').where('age','>',18).orderBy('name').limit(10)`) and **TypeORM**'s `createQueryBuilder()`. Write down: which methods return `this` (fluent), where the "build/execute" boundary is (`.toSQL()` / `.getMany()`), and whether they expose a Director-like recipe concept. Relate each back to a participant in the README's diagram.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
