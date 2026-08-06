/**
 * SINGLETON PATTERN — Production-style TypeScript example
 * -------------------------------------------------------
 * This file shows the THREE ways "one instance per process" appears in real
 * Node/TypeScript backends, from the textbook mechanism to what you should
 * actually ship:
 *
 *   1. Classic getInstance()  -> ConfigManager, PgConnectionPool
 *                                (private ctor + static field + lazy accessor)
 *   2. Node module-cache       -> `configSingleton` export
 *                                (top-level `new` is a de-facto singleton)
 *   3. DI-managed singleton    -> AppConfigService + ReportService
 *                                (the RECOMMENDED production approach)
 *
 * Key lessons baked in:
 *   - lazy vs eager initialization
 *   - async init race + how to avoid it (cache the PROMISE, not the result)
 *   - explicit lifecycle (close()) for resource-holding singletons
 *   - a test-only reset() and, better, DI to keep tests isolated
 *   - a singleton is PER-PROCESS, not per-cluster/per-pod
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. CLASSIC LAZY SINGLETON — application configuration.
//    Legitimate use: config is loaded once, immutable, read everywhere.
// =============================================================================

/** Read-only view of config the rest of the app depends on. */
export interface AppConfig {
  get(key: string): string | undefined;
  getNumber(key: string): number | undefined;
  getBoolean(key: string): boolean;
}

export class ConfigManager implements AppConfig {
  // The single instance lives in a static field => one slot for the whole class.
  private static instance: ConfigManager | undefined;

  private readonly values: Map<string, string>;

  // Private constructor: `new ConfigManager()` is illegal outside this class.
  // The expensive one-time work (read + validate) happens here, exactly once.
  private constructor() {
    this.values = new Map<string, string>();

    // In real life: read process.env, secrets manager, config files, etc.
    // We seed a few sensible defaults so the demo is deterministic.
    const source: Record<string, string> = {
      NODE_ENV: process.env.NODE_ENV ?? "development",
      DB_HOST: process.env.DB_HOST ?? "localhost",
      DB_POOL_SIZE: process.env.DB_POOL_SIZE ?? "10",
      FEATURE_NEW_BILLING: process.env.FEATURE_NEW_BILLING ?? "false",
    };
    for (const [k, v] of Object.entries(source)) this.values.set(k, v);

    // Fail fast on invalid config at construction time, not deep in a request.
    const poolSize = Number(this.values.get("DB_POOL_SIZE"));
    if (!Number.isInteger(poolSize) || poolSize <= 0) {
      throw new Error(`Invalid DB_POOL_SIZE: ${this.values.get("DB_POOL_SIZE")}`);
    }
  }

  /** Lazy accessor: create on first call, reuse forever after. */
  public static getInstance(): ConfigManager {
    if (!ConfigManager.instance) {
      ConfigManager.instance = new ConfigManager();
    }
    return ConfigManager.instance;
  }

  public get(key: string): string | undefined {
    return this.values.get(key);
  }

  public getNumber(key: string): number | undefined {
    const raw = this.values.get(key);
    return raw === undefined ? undefined : Number(raw);
  }

  public getBoolean(key: string): boolean {
    return this.values.get(key) === "true";
  }

  /**
   * TEST-ONLY escape hatch. Its very existence is a smell: it exists because a
   * classic singleton's static state leaks between tests. The cleaner fix is DI
   * (see AppConfigService below). Keep it internal / guard it in real code.
   */
  public static resetForTests(): void {
    ConfigManager.instance = undefined;
  }
}

// =============================================================================
// 2. RESOURCE-HOLDING SINGLETON — a database connection pool.
//    Textbook justification: exactly ONE bounded pool per process so we never
//    exhaust PostgreSQL's max_connections. Note the ASYNC init and lifecycle.
// =============================================================================

/** Stand-in for a real driver pool (e.g. pg.Pool). */
interface PoolConnection {
  id: number;
}

export class PgConnectionPool {
  private static instance: PgConnectionPool | undefined;
  // We cache the initialization PROMISE, not just the instance, so that two
  // concurrent first-time callers await the SAME setup instead of opening two
  // pools (the async-init race described in the README).
  private static initPromise: Promise<PgConnectionPool> | undefined;

  private connections: PoolConnection[] = [];
  private closed = false;

  private constructor(private readonly size: number) {}

  /** Async, race-safe lazy accessor. */
  public static getInstance(): Promise<PgConnectionPool> {
    if (PgConnectionPool.instance) {
      return Promise.resolve(PgConnectionPool.instance);
    }
    if (!PgConnectionPool.initPromise) {
      PgConnectionPool.initPromise = PgConnectionPool.build();
    }
    return PgConnectionPool.initPromise;
  }

  private static async build(): Promise<PgConnectionPool> {
    const size = ConfigManager.getInstance().getNumber("DB_POOL_SIZE") ?? 10;
    const pool = new PgConnectionPool(size);
    // Simulate opening TCP connections + auth (the expensive part).
    for (let i = 0; i < pool.size; i++) {
      await Promise.resolve();
      pool.connections.push({ id: i });
    }
    PgConnectionPool.instance = pool;
    return pool;
  }

  public async query<T>(sql: string, mapRow: () => T): Promise<T> {
    if (this.closed) throw new Error("Pool is closed");
    // Borrow a connection, run the query, return it. (Simplified.)
    void sql;
    await Promise.resolve();
    return mapRow();
  }

  /** Explicit lifecycle: drain the pool on shutdown (SIGTERM). */
  public async close(): Promise<void> {
    this.closed = true;
    this.connections = [];
    PgConnectionPool.instance = undefined;
    PgConnectionPool.initPromise = undefined;
  }

  public get openConnections(): number {
    return this.connections.length;
  }
}

// =============================================================================
// 3. NODE MODULE-CACHE SINGLETON.
//    Because Node caches modules by resolved path, a top-level `new` runs once
//    and every importer receives the SAME object — no boilerplate required.
//    This is the idiomatic Node "singleton" for simple shared objects.
// =============================================================================

/**
 * `export const configSingleton = ...` — any file that imports this gets the
 * identical reference. Equivalent idiom: `export default new SomeSimpleThing()`.
 * Great for stateless/config objects; risky for mutable state and hard to mock.
 */
export const configSingleton: AppConfig = ConfigManager.getInstance();

// =============================================================================
// 4. DI-MANAGED SINGLETON — the RECOMMENDED production approach (NestJS style).
//    Note what is MISSING: no private constructor, no static field, no
//    getInstance(). It is a normal class with a normal constructor.
// =============================================================================

/**
 * In a real NestJS app this class carries `@Injectable()`. Nest's default
 * provider scope is DEFAULT (singleton): the container instantiates it ONCE per
 * application and injects the same instance everywhere it is requested.
 * (Decorator omitted so this file runs without the @nestjs/common dependency.)
 */
export class AppConfigService implements AppConfig {
  private readonly values = new Map<string, string>([
    ["NODE_ENV", process.env.NODE_ENV ?? "development"],
    ["REPORT_FORMAT", process.env.REPORT_FORMAT ?? "pdf"],
  ]);

  get(key: string): string | undefined {
    return this.values.get(key);
  }
  getNumber(key: string): number | undefined {
    const raw = this.values.get(key);
    return raw === undefined ? undefined : Number(raw);
  }
  getBoolean(key: string): boolean {
    return this.values.get(key) === "true";
  }
}

/**
 * A consumer. It DECLARES its dependency in the constructor (explicit, honest)
 * and never calls getInstance(). In production Nest injects the single
 * AppConfigService; in tests you hand it a fake — trivial to unit-test.
 */
export class ReportService {
  constructor(private readonly config: AppConfig) {}

  buildReport(): string {
    const format = this.config.get("REPORT_FORMAT");
    const env = this.config.get("NODE_ENV");
    return `Report generated as ${format} (env=${env})`;
  }
}

// A tiny hand-rolled "container" to illustrate the wiring Nest does for you:
// build the shared instance ONCE, inject it everywhere.
function buildProductionGraph() {
  const config = new AppConfigService(); // container creates ONE
  const reportService = new ReportService(config); // and injects it
  return { config, reportService };
}

// =============================================================================
// 5. DEMO — proves single-instance guarantees and correct lifecycle.
// =============================================================================

async function main(): Promise<void> {
  // --- Classic singleton: two lookups return the SAME reference ---
  const a = ConfigManager.getInstance();
  const b = ConfigManager.getInstance();
  console.log("[classic] same instance?", a === b); // true
  console.log("[classic] DB_HOST:", a.get("DB_HOST"));

  // --- Module-cache singleton is the same object as getInstance() here ---
  console.log("[module] same as getInstance?", configSingleton === a); // true

  // --- Resource singleton: race-safe async init, then shared usage ---
  const [p1, p2] = await Promise.all([
    PgConnectionPool.getInstance(),
    PgConnectionPool.getInstance(),
  ]);
  console.log("[pool] same instance despite concurrent init?", p1 === p2); // true
  console.log("[pool] open connections:", p1.openConnections);
  const row = await p1.query("SELECT 1", () => ({ ok: true }));
  console.log("[pool] query result:", row);

  // --- DI-managed singleton: consumer uses injected config, no global fetch ---
  const graph = buildProductionGraph();
  console.log("[DI]", graph.reportService.buildReport());

  // In a test we could instead do:
  //   const fake: AppConfig = { get: () => "csv", getNumber: () => 0, getBoolean: () => false };
  //   const svc = new ReportService(fake);
  // ...with zero changes to ReportService. That is the payoff of DI over getInstance().

  // --- Graceful shutdown: resource singletons MUST be closed ---
  await p1.close();
  console.log("[pool] closed. open connections:", p1.openConnections);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
