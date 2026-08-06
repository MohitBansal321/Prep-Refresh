# Abstract Factory Pattern — Exercises

Work through these in order. Do not look at any solution; the goal is to build the reflex of spotting when you need a *family* of related objects (not one product) and concentrating the family choice into a single factory.

> Rule of thumb for every exercise: pick a **family**, never individual products. Your abstract product interfaces must be designed from *your application's needs*, never copied from a vendor's SDK. One concrete factory must return **only** its own family's products — mixing families is a bug the type system should forbid.

---

## Easy — Two-Themed UI Widget Family

Your app renders a tiny UI made of two related widgets and expects these interfaces:

```ts
interface Button {
  render(): string; // returns markup/text for the button
}
interface Checkbox {
  render(): string;
}
interface WidgetFactory {
  createButton(): Button;
  createCheckbox(): Checkbox;
}
```

**Task:** Implement two families — `LightThemeFactory` and `DarkThemeFactory` — each producing a matching `Button` + `Checkbox` whose `render()` output reflects the theme (e.g. `"[light button]"` vs `"[dark button]"`). Write a client function `buildForm(factory: WidgetFactory)` that renders both widgets and never mentions a concrete class.

**Acceptance:** Passing `LightThemeFactory` renders two light widgets; passing `DarkThemeFactory` renders two dark ones. `buildForm` is unchanged between the two runs.

---

## Medium — Cross-Database Persistence Family

Your app supports PostgreSQL and MySQL. Each needs a matching trio that speaks its own dialect:

```ts
interface Connection {
  open(): void;
  dialect(): string;
}
interface QueryBuilder {
  select(table: string): string; // returns dialect-specific SQL
}
interface Migrator {
  up(): string;
}
interface PersistenceFactory {
  createConnection(): Connection;
  createQueryBuilder(): QueryBuilder;
  createMigrator(): Migrator;
}
```

**Task:**
- Implement `PostgresFactory` and `MySqlFactory`, each returning a consistent `{Connection, QueryBuilder, Migrator}` for its dialect (e.g. Postgres identifiers quoted with `"..."`, MySQL with backticks; limit syntax differs).
- Add a composition-root function `buildPersistence(db: "postgres" | "mysql")` that is the **only** place the dialect is chosen.

**Bonus constraint:** Thread a shared `DbConfig` (host, credentials) through the factory constructor so all three products inherit the same connection settings automatically.

---

## Hard — Family Consistency Enforced by Types

Extend the Medium exercise so that a mismatch is a **compile-time** error, not a runtime one.

**Task:**
- Add an exhaustiveness `switch` (using a `never` check) in `buildPersistence` so that adding a third dialect (`"sqlite"`) to the union without wiring a factory branch fails to compile.
- Prove that it is *structurally impossible* for `PostgresFactory.createQueryBuilder()` to return a MySQL query builder — explain in a comment which language feature guarantees this.
- Write a client `runReport(factory: PersistenceFactory)` that opens the connection, builds a query, and runs a migration using only the abstract interfaces, then assert that `connection.dialect()` and the query's quoting style always agree.

**Think about:** Where is consistency guaranteed — in the client, or by the fact that all three products came from one factory? What would break the guarantee?

---

## Real-World Challenge — Provider-Agnostic Cloud Infrastructure

Rebuild the scenario from [code.ts](code.ts) from scratch (do not copy it) for a service that ingests events.

```ts
interface BlobStorage {
  put(key: string, data: Buffer, contentType: string): Promise<{ url: string }>;
  get(key: string): Promise<Buffer>;
}
interface MessageQueue {
  publish(topic: string, message: string): Promise<{ messageId: string }>;
  consume(topic: string): Promise<string | null>;
}
interface SqlDatabase {
  connect(): Promise<void>;
  query<T>(sql: string, params?: unknown[]): Promise<T[]>;
}
interface CloudResourceFactory {
  createBlobStorage(): BlobStorage;
  createMessageQueue(): MessageQueue;
  createDatabase(): SqlDatabase;
}
```

**Requirements:**
- Provide two families against *simulated* SDKs (no real cloud calls): `AwsFactory` (S3 + SQS + RDS) and `GcpFactory` (GCS + Pub/Sub + Cloud SQL).
- Business code (`EventPipelineService.process(event)`) depends only on the abstract factory and abstract products; it must contain no `if (provider === ...)` and no `new` of a concrete product.
- Selection of the family happens exclusively at a composition root, driven by an env var (`CLOUD_PROVIDER`).
- Every product in a family shares one `CloudConfig` (region + credentials) threaded through the factory.
- Write one unit test for `EventPipelineService` using an in-memory `FakeFactory` returning fake products — no cloud, no credentials. This proves the decoupling.

**Stretch:** Add a **fourth product type** — `SecretManager` — to `CloudResourceFactory`. Notice how many files you must touch, and write down *why* adding a product type is expensive while adding a family (Azure) is cheap. This exercises the pattern's central rigidity tradeoff.

---

## Bonus Challenge — Test Family, New Family, and the Two Axes

1. Build a `FakeFactory` returning fully in-memory `{BlobStorage, MessageQueue, SqlDatabase}` (a `Map`-backed store, an array-backed queue, an in-memory table). Swap it for the real family across your whole test suite by changing a **single** line. Confirm no business code changes.

2. Add a **new family** `AzureFactory` (Blob Storage + Service Bus + Azure SQL). Confirm you did **not** modify `EventPipelineService` or any existing factory — only added new files and one branch at the composition root. Write down which SOLID principles this demonstrates (hint: Open/Closed).

3. Contrast the two directions of change in a short note:
   - Adding a new **family** (Azure) — how many existing files change?
   - Adding a new **product type** (`SecretManager`) — how many existing files change, and why?
   State the rule you would give a teammate for *when* Abstract Factory is the right choice based on which axis you expect to grow.

4. Explain, in two sentences, the crisp difference between **Factory Method** and **Abstract Factory** using this scenario — one product via subclassing vs a family via one object with several `create*()` methods.

---

*Solutions are intentionally omitted. Ask for a specific exercise's solution if you want it walked through.*
