/**
 * PROXY PATTERN — Production-style TypeScript example
 * ---------------------------------------------------
 * Scenario: Our analytics dashboard shows a "monthly revenue report". Generating
 * that report means running a heavy SQL aggregation across millions of rows — it
 * can take several seconds and hammers PostgreSQL. Many users open the same
 * dashboard, so we recompute the SAME report over and over.
 *
 * We also must ensure only users with the "analyst" role can read a report.
 *
 * We solve BOTH problems with the Proxy Pattern, WITHOUT changing the real service
 * and WITHOUT the caller ever knowing a proxy exists:
 *
 *   - ReportService          -> Subject      (the common interface everyone speaks)
 *   - DatabaseReportService  -> RealSubject  (the expensive object doing real work)
 *   - CachingReportProxy     -> Proxy (smart/caching + virtual/lazy)
 *   - ProtectionReportProxy  -> Proxy (protection / access control)
 *   - ReportController       -> Client (depends ONLY on ReportService)
 *
 * Because every proxy implements the SAME interface as the RealSubject, we can even
 * STACK them: Protection -> Caching -> Real. The client cannot tell the difference.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. SUBJECT — the common interface. Client, RealSubject and every Proxy all
//    implement THIS. This is what makes a proxy transparent to the client.
// =============================================================================

export interface Report {
  id: string;
  generatedAt: string; // ISO timestamp — lets us see cache hits vs fresh builds
  totalRevenueInCents: number;
  rowsScanned: number;
}

/** The role of whoever is asking. In real life this comes from the auth layer. */
export interface Requester {
  userId: string;
  roles: string[];
}

/**
 * The Subject. The client depends only on this. It has no idea whether the object
 * behind it is the real database service, a cache, an access-control gate, or all
 * three stacked together.
 */
export interface ReportService {
  generateReport(reportId: string, requester: Requester): Promise<Report>;
}

/** Our own domain error, thrown by the protection proxy. */
export class AccessDeniedError extends Error {
  constructor(userId: string, reportId: string) {
    super(`User "${userId}" is not allowed to read report "${reportId}"`);
    this.name = "AccessDeniedError";
  }
}

// =============================================================================
// 2. REAL SUBJECT — the object that does the actual, expensive work.
//    It is deliberately "dumb": it knows nothing about caching or auth. Its ONLY
//    job is to compute the report correctly (Single Responsibility Principle).
// =============================================================================

/** A tiny abstraction over the database so we can inject a fake in tests. */
export interface QueryRunner {
  aggregateRevenue(reportId: string): Promise<{ totalInCents: number; rowsScanned: number }>;
}

export class DatabaseReportService implements ReportService {
  constructor(private readonly db: QueryRunner) {}

  async generateReport(reportId: string, _requester: Requester): Promise<Report> {
    // Pretend this is a multi-second heavy aggregation over PostgreSQL.
    console.log(`[RealSubject] Running EXPENSIVE aggregation for "${reportId}"...`);
    const { totalInCents, rowsScanned } = await this.db.aggregateRevenue(reportId);

    return {
      id: reportId,
      generatedAt: new Date().toISOString(),
      totalRevenueInCents: totalInCents,
      rowsScanned,
    };
  }
}

// =============================================================================
// 3. CACHE PORT — modelled on Redis. Injected into the caching proxy so the proxy
//    is testable (in tests we pass an in-memory implementation, no Redis needed).
// =============================================================================

export interface Cache {
  get(key: string): Promise<string | null>;
  set(key: string, value: string, ttlSeconds: number): Promise<void>;
  del(key: string): Promise<void>;
}

/** A minimal in-memory stand-in for Redis (fine for local runs and unit tests). */
export class InMemoryCache implements Cache {
  private store = new Map<string, { value: string; expiresAt: number }>();

  async get(key: string): Promise<string | null> {
    const entry = this.store.get(key);
    if (!entry) return null;
    if (Date.now() > entry.expiresAt) {
      this.store.delete(key); // lazy expiry
      return null;
    }
    return entry.value;
  }

  async set(key: string, value: string, ttlSeconds: number): Promise<void> {
    this.store.set(key, { value, expiresAt: Date.now() + ttlSeconds * 1000 });
  }

  async del(key: string): Promise<void> {
    this.store.delete(key);
  }
}

// =============================================================================
// 4. CACHING PROXY (smart proxy + virtual proxy).
//    - Smart:   caches the RealSubject's result so we don't recompute it.
//    - Virtual: lazily creates the RealSubject only on the FIRST cache miss,
//               so if every request is a cache hit we never even build it.
//    Same interface as the RealSubject => the client cannot tell it apart.
// =============================================================================

export class CachingReportProxy implements ReportService {
  /** Lazily created RealSubject. Undefined until the first cache miss. */
  private realService?: ReportService;

  constructor(
    // A FACTORY, not the instance itself. This is what enables lazy creation:
    // the proxy decides *when* the real object comes into existence.
    private readonly realServiceFactory: () => ReportService,
    private readonly cache: Cache,
    private readonly ttlSeconds: number = 60,
  ) {}

  async generateReport(reportId: string, requester: Requester): Promise<Report> {
    const cacheKey = `report:${reportId}`;

    // 1) Try the cache first — this is the whole point of the proxy.
    const cached = await this.cache.get(cacheKey);
    if (cached) {
      console.log(`[CachingProxy] HIT for "${reportId}" — real service NOT called.`);
      return JSON.parse(cached) as Report;
    }

    // 2) Cache miss. Lazily build the RealSubject the first time we truly need it.
    console.log(`[CachingProxy] MISS for "${reportId}".`);
    const report = await this.getRealService().generateReport(reportId, requester);

    // 3) Store for next time, then return.
    await this.cache.set(cacheKey, JSON.stringify(report), this.ttlSeconds);
    return report;
  }

  /** Cache invalidation lives with the proxy — the client should not manage keys. */
  async invalidate(reportId: string): Promise<void> {
    await this.cache.del(`report:${reportId}`);
    console.log(`[CachingProxy] Invalidated "${reportId}".`);
  }

  private getRealService(): ReportService {
    if (!this.realService) {
      console.log(`[CachingProxy] Lazily instantiating the RealSubject now.`);
      this.realService = this.realServiceFactory();
    }
    return this.realService;
  }
}

// =============================================================================
// 5. PROTECTION PROXY (access control).
//    Enforces authorization BEFORE delegating. It does no business work itself —
//    it only decides whether the call is allowed through. Same interface again.
// =============================================================================

export class ProtectionReportProxy implements ReportService {
  constructor(
    private readonly next: ReportService, // could be the real subject OR another proxy
    private readonly requiredRole: string = "analyst",
  ) {}

  async generateReport(reportId: string, requester: Requester): Promise<Report> {
    if (!requester.roles.includes(this.requiredRole)) {
      console.log(`[ProtectionProxy] DENIED user "${requester.userId}".`);
      throw new AccessDeniedError(requester.userId, reportId);
    }
    console.log(`[ProtectionProxy] Allowed user "${requester.userId}". Delegating.`);
    return this.next.generateReport(reportId, requester);
  }
}

// =============================================================================
// 6. CLIENT — depends ONLY on the Subject interface. It never learns whether it
//    is talking to the real service, a cache, or an auth gate.
// =============================================================================

export class ReportController {
  constructor(private readonly service: ReportService) {}

  async handleRequest(reportId: string, requester: Requester): Promise<Report> {
    const report = await this.service.generateReport(reportId, requester);
    console.log(
      `[Client] Served report "${report.id}" (generatedAt=${report.generatedAt}).`,
    );
    return report;
  }
}

// =============================================================================
// 7. NATIVE JS PROXY — the language-level realization of this exact pattern.
//    The built-in `Proxy` object intercepts operations (here: property access)
//    on a target, transparently, without changing the target's class.
// =============================================================================

export function withAccessLogging<T extends object>(target: T, label: string): T {
  return new Proxy(target, {
    get(obj, prop, receiver) {
      const value = Reflect.get(obj, prop, receiver);
      if (typeof value === "function") {
        return (...args: unknown[]) => {
          console.log(`[JS Proxy:${label}] called .${String(prop)}()`);
          return (value as (...a: unknown[]) => unknown).apply(obj, args);
        };
      }
      return value;
    },
  });
}

// =============================================================================
// 8. COMPOSITION ROOT — wire the stack. Notice we can STACK proxies because they
//    all share the Subject interface: Protection -> Caching -> Real.
// =============================================================================

function buildReportService(): ReportService {
  const fakeDb: QueryRunner = {
    // Simulated heavy query result.
    aggregateRevenue: async (reportId) => ({
      totalInCents: 123_456_789,
      rowsScanned: 4_200_000,
    }),
  };

  const cache = new InMemoryCache();

  // Virtual + smart proxy. The RealSubject is created lazily via the factory.
  const caching = new CachingReportProxy(
    () => new DatabaseReportService(fakeDb),
    cache,
    60,
  );

  // Protection proxy sits OUTERMOST: auth is checked before we even hit the cache.
  return new ProtectionReportProxy(caching, "analyst");
}

// =============================================================================
// 9. DEMO — the client code is identical regardless of how many proxies are stacked.
// =============================================================================

async function main(): Promise<void> {
  const service = buildReportService();
  const controller = new ReportController(service);

  const analyst: Requester = { userId: "alice", roles: ["analyst"] };
  const guest: Requester = { userId: "bob", roles: ["guest"] };

  console.log("\n--- 1st call (analyst): expect MISS + expensive build ---");
  await controller.handleRequest("revenue-2026-06", analyst);

  console.log("\n--- 2nd call (analyst): expect HIT, no expensive build ---");
  await controller.handleRequest("revenue-2026-06", analyst);

  console.log("\n--- 3rd call (guest): expect DENIED before reaching cache/DB ---");
  try {
    await controller.handleRequest("revenue-2026-06", guest);
  } catch (err) {
    console.log(`[Client] Caught: ${(err as Error).message}`);
  }

  console.log("\n--- Native JS Proxy: transparent method-call logging ---");
  const logged = withAccessLogging(new InMemoryCache(), "cache");
  await logged.set("k", "v", 30);
  console.log("value:", await logged.get("k"));
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
