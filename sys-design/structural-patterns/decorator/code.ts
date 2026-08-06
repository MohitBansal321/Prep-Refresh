/**
 * DECORATOR PATTERN — Production-style TypeScript example
 * -------------------------------------------------------
 * Scenario: Almost every backend service makes outbound HTTP calls (to other
 * microservices, third-party APIs, webhooks). Around those raw calls we always
 * need cross-cutting concerns: logging, retries, caching, rate limiting.
 *
 * We do NOT want a single monolithic HttpClient that hard-codes all of those
 * concerns (that class would be untestable and violate the Single Responsibility
 * Principle). We also do NOT want to explode into subclasses like
 * `LoggingRetryingCachingHttpClient`.
 *
 * Instead we use the Decorator Pattern: each concern is a small wrapper that
 * implements the SAME `HttpClient` interface as the thing it wraps, does a bit
 * of work before/after, and delegates the rest inward. Because every layer has
 * the same shape, we can STACK them in any order and nest them recursively.
 *
 *   HttpClient          -> Component interface (the shared contract)
 *   FetchHttpClient     -> ConcreteComponent  (the real network call)
 *   HttpClientDecorator -> Decorator          (abstract base; holds an HttpClient)
 *   LoggingHttpClient
 *   RetryHttpClient
 *   CacheHttpClient     -> ConcreteDecorators (add ONE responsibility each)
 *   RateLimitHttpClient
 *
 * The whole point of this file is the last section: the SAME four decorators,
 * stacked in two DIFFERENT orders, produce DIFFERENT behaviour. Order matters.
 *
 * Run with: npx ts-node code.ts   (or compile with tsc and run the .js)
 */

// =============================================================================
// 0. Small injectable collaborators (kept injectable for testability / SOLID).
//    In real code these would come from your DI container (e.g. NestJS providers).
// =============================================================================

/** Minimal logger seam so tests can capture output instead of hitting the console. */
export interface Logger {
  log(message: string): void;
}

/** A sleep function is injected so tests can fake time instead of really waiting. */
export type Sleep = (ms: number) => Promise<void>;
export const realSleep: Sleep = (ms) => new Promise((resolve) => setTimeout(resolve, ms));

/** A clock seam so the rate limiter and cache TTL are deterministic in tests. */
export type Clock = () => number; // epoch milliseconds
export const realClock: Clock = () => Date.now();

// =============================================================================
// 1. COMPONENT INTERFACE — the shared contract.
//    EVERY participant (the real client AND every decorator) implements THIS.
//    Keeping the interface identical is what lets decorators stack.
// =============================================================================

export interface HttpRequest {
  method: "GET" | "POST" | "PUT" | "DELETE" | "PATCH";
  url: string;
  headers?: Record<string, string>;
  body?: unknown;
}

export interface HttpResponse {
  status: number;
  body: unknown;
  headers: Record<string, string>;
}

/** The Component. This is the ONLY type the rest of the application depends on. */
export interface HttpClient {
  send(request: HttpRequest): Promise<HttpResponse>;
}

// =============================================================================
// 2. CONCRETE COMPONENT — the real object being decorated.
//    This is the actual network call. It has NO knowledge of logging, retries,
//    caching or rate limiting. It does exactly one thing.
// =============================================================================

/**
 * A stand-in for a real HTTP client (fetch/axios/undici). To make retries and
 * caching visible in the demo, it deterministically fails the first attempt for
 * any URL containing "flaky", then succeeds. It also counts how many times it
 * was actually hit, so we can PROVE that caching and rate limiting change the
 * number of real network calls.
 */
export class FetchHttpClient implements HttpClient {
  public networkCalls = 0; // observable for the demo/tests
  private attemptsByUrl = new Map<string, number>();

  async send(request: HttpRequest): Promise<HttpResponse> {
    this.networkCalls++;

    const attempt = (this.attemptsByUrl.get(request.url) ?? 0) + 1;
    this.attemptsByUrl.set(request.url, attempt);

    // Simulate a transient failure on the first try for "flaky" endpoints.
    if (request.url.includes("flaky") && attempt === 1) {
      throw new Error(`503 Service Unavailable (transient) for ${request.url}`);
    }

    // Simulate a successful response.
    return {
      status: 200,
      body: { url: request.url, servedAt: Date.now() },
      headers: { "content-type": "application/json" },
    };
  }
}

// =============================================================================
// 3. DECORATOR (abstract) — the heart of the pattern.
//    - It IMPLEMENTS the same Component interface (HttpClient).
//    - It HOLDS a reference to another HttpClient ("inner" / the wrapped object).
//    - Its default behaviour is pure delegation. Subclasses override send() to
//      add behaviour BEFORE and/or AFTER calling super.send()/this.inner.send().
// =============================================================================

export abstract class HttpClientDecorator implements HttpClient {
  // `protected` so concrete decorators can reach the wrapped client.
  // Typed as the INTERFACE, never a concrete class — this is why any decorator
  // can wrap any other decorator (or the base client).
  protected constructor(protected readonly inner: HttpClient) {}

  // Default = transparent pass-through. A decorator that overrides nothing
  // behaves exactly like the object it wraps.
  send(request: HttpRequest): Promise<HttpResponse> {
    return this.inner.send(request);
  }
}

// =============================================================================
// 4. CONCRETE DECORATORS — each adds exactly ONE responsibility.
// =============================================================================

/**
 * LOGGING decorator.
 * Adds observability around whatever it wraps: method, url, status, latency,
 * and errors. Note it measures the time of everything INSIDE it — so if it sits
 * OUTSIDE the retry decorator, the reported latency includes all retry attempts.
 */
export class LoggingHttpClient extends HttpClientDecorator {
  constructor(inner: HttpClient, private readonly logger: Logger = console, private readonly clock: Clock = realClock) {
    super(inner);
  }

  async send(request: HttpRequest): Promise<HttpResponse> {
    const start = this.clock();
    this.logger.log(`--> ${request.method} ${request.url}`);
    try {
      const response = await this.inner.send(request);
      this.logger.log(`<-- ${response.status} ${request.url} (${this.clock() - start}ms)`);
      return response;
    } catch (error) {
      this.logger.log(`x-- FAILED ${request.url} (${this.clock() - start}ms): ${(error as Error).message}`);
      throw error; // never swallow — a logging decorator must be transparent to control flow
    }
  }
}

/**
 * RETRY decorator.
 * Re-attempts the wrapped call with exponential backoff on failure.
 * IMPORTANT: only retries idempotent methods. Retrying a non-idempotent POST
 * could double-charge a customer, so we refuse — this is the kind of real-world
 * correctness rule that belongs in the decorator, not the business logic.
 */
export class RetryHttpClient extends HttpClientDecorator {
  private static readonly IDEMPOTENT = new Set(["GET", "PUT", "DELETE"]);

  constructor(
    inner: HttpClient,
    private readonly opts: { maxRetries: number; baseDelayMs: number },
    private readonly sleep: Sleep = realSleep,
  ) {
    super(inner);
  }

  async send(request: HttpRequest): Promise<HttpResponse> {
    if (!RetryHttpClient.IDEMPOTENT.has(request.method)) {
      return this.inner.send(request); // do NOT retry unsafe methods
    }

    let lastError: unknown;
    for (let attempt = 0; attempt <= this.opts.maxRetries; attempt++) {
      try {
        return await this.inner.send(request);
      } catch (error) {
        lastError = error;
        if (attempt < this.opts.maxRetries) {
          const delay = this.opts.baseDelayMs * 2 ** attempt; // 1x, 2x, 4x ...
          await this.sleep(delay);
        }
      }
    }
    throw lastError;
  }
}

/** Pluggable cache backend so we can use Redis in prod and a Map in tests. */
export interface CacheStore {
  get(key: string): Promise<HttpResponse | null>;
  set(key: string, value: HttpResponse, ttlMs: number): Promise<void>;
}

/** Default in-memory store. In production you would inject a Redis-backed one. */
export class InMemoryCacheStore implements CacheStore {
  private readonly map = new Map<string, { value: HttpResponse; expiresAt: number }>();
  constructor(private readonly clock: Clock = realClock) {}

  async get(key: string): Promise<HttpResponse | null> {
    const entry = this.map.get(key);
    if (!entry) return null;
    if (this.clock() > entry.expiresAt) {
      this.map.delete(key);
      return null;
    }
    return entry.value;
  }

  async set(key: string, value: HttpResponse, ttlMs: number): Promise<void> {
    this.map.set(key, { value, expiresAt: this.clock() + ttlMs });
  }
}

/**
 * CACHE decorator.
 * Serves cached GET responses and short-circuits everything inside it on a hit.
 * A cache HIT never calls `this.inner.send`, so whatever is nested below (retry,
 * rate limit, network) is skipped entirely. That is exactly why the POSITION of
 * this decorator in the stack changes the system's behaviour so dramatically.
 */
export class CacheHttpClient extends HttpClientDecorator {
  constructor(inner: HttpClient, private readonly store: CacheStore, private readonly ttlMs: number) {
    super(inner);
  }

  async send(request: HttpRequest): Promise<HttpResponse> {
    if (request.method !== "GET") return this.inner.send(request); // only cache safe reads

    const key = `${request.method} ${request.url}`;
    const cached = await this.store.get(key);
    if (cached) {
      return { ...cached, headers: { ...cached.headers, "x-cache": "HIT" } };
    }

    const response = await this.inner.send(request);
    if (response.status >= 200 && response.status < 300) {
      await this.store.set(key, response, this.ttlMs);
    }
    return { ...response, headers: { ...response.headers, "x-cache": "MISS" } };
  }
}

/**
 * RATE-LIMIT decorator (token bucket).
 * Ensures we do not exceed `capacity` requests per refill window against the
 * thing it wraps. Because it only spends a token when it actually calls inward,
 * placing it INSIDE the cache means cache hits cost no tokens — the sensible
 * choice. Placing it OUTSIDE the cache would waste tokens on cache hits.
 */
export class RateLimitHttpClient extends HttpClientDecorator {
  private tokens: number;
  private lastRefill: number;

  constructor(
    inner: HttpClient,
    private readonly opts: { capacity: number; refillPerSecond: number },
    private readonly clock: Clock = realClock,
    private readonly sleep: Sleep = realSleep,
  ) {
    super(inner);
    this.tokens = opts.capacity;
    this.lastRefill = clock();
  }

  private refill(): void {
    const now = this.clock();
    const elapsedSeconds = (now - this.lastRefill) / 1000;
    const replenished = elapsedSeconds * this.opts.refillPerSecond;
    if (replenished >= 1) {
      this.tokens = Math.min(this.opts.capacity, this.tokens + Math.floor(replenished));
      this.lastRefill = now;
    }
  }

  async send(request: HttpRequest): Promise<HttpResponse> {
    this.refill();
    while (this.tokens < 1) {
      await this.sleep(1000 / this.opts.refillPerSecond); // wait for a token
      this.refill();
    }
    this.tokens--;
    return this.inner.send(request);
  }
}

// =============================================================================
// 5. CLIENT — business logic depends ONLY on the HttpClient interface.
//    It cannot tell whether it holds a bare FetchHttpClient or a 4-deep stack.
// =============================================================================

export class UserApiClient {
  constructor(private readonly http: HttpClient) {}

  async getUser(id: string): Promise<HttpResponse> {
    return this.http.send({ method: "GET", url: `https://api.example.com/users/${id}` });
  }

  async getFlakyResource(): Promise<HttpResponse> {
    return this.http.send({ method: "GET", url: "https://api.example.com/flaky/resource" });
  }
}

// =============================================================================
// 6. COMPOSITION ROOT — build the decorator stacks.
//    The order of wrapping IS the configuration. Same parts, different order,
//    different behaviour. This is what makes Decorator powerful (and dangerous).
// =============================================================================

/**
 * RECOMMENDED order (outer -> inner):
 *   Logging -> Cache -> Retry -> RateLimit -> Fetch
 *
 * Read it from the outside in:
 *   - Logging is outermost, so it measures TOTAL time and sees cache hits.
 *   - Cache is next, so a hit short-circuits retry, rate limit AND the network.
 *   - Retry sits above RateLimit, so every retry attempt still respects the limit.
 *   - RateLimit is closest to the network, so only real calls spend tokens.
 */
export function buildRecommendedClient(base: FetchHttpClient, logger: Logger, sleep: Sleep): HttpClient {
  return new LoggingHttpClient(
    new CacheHttpClient(
      new RetryHttpClient(
        new RateLimitHttpClient(base, { capacity: 5, refillPerSecond: 5 }, realClock, sleep),
        { maxRetries: 2, baseDelayMs: 1 },
        sleep,
      ),
      new InMemoryCacheStore(),
      60_000,
    ),
    logger,
  );
}

/**
 * A DELIBERATELY WORSE order to demonstrate that order matters:
 *   Cache -> Logging -> RateLimit -> Retry -> Fetch
 *
 * Consequences of this arrangement:
 *   - Logging is now INSIDE the cache, so a cache hit logs NOTHING (you lose
 *     visibility into how often you serve from cache).
 *   - RateLimit is now ABOVE Retry, so a single logical request that needs 2
 *     retries spends only ONE token for all attempts (arguably wrong — a burst
 *     of retries can hammer a struggling downstream without limit at the attempt
 *     level).
 */
export function buildQuestionableClient(base: FetchHttpClient, logger: Logger, sleep: Sleep): HttpClient {
  return new CacheHttpClient(
    new LoggingHttpClient(
      new RateLimitHttpClient(
        new RetryHttpClient(base, { maxRetries: 2, baseDelayMs: 1 }, sleep),
        { capacity: 5, refillPerSecond: 5 },
        realClock,
        sleep,
      ),
      logger,
    ),
    new InMemoryCacheStore(),
    60_000,
  );
}

// =============================================================================
// 7. DEMO
// =============================================================================

async function main(): Promise<void> {
  const fastSleep: Sleep = () => Promise.resolve(); // don't actually wait in the demo

  console.log("=== RECOMMENDED stack: Logging -> Cache -> Retry -> RateLimit -> Fetch ===\n");
  const base1 = new FetchHttpClient();
  const client1 = new UserApiClient(buildRecommendedClient(base1, console, fastSleep));

  await client1.getUser("42"); // MISS -> network
  await client1.getUser("42"); // HIT  -> short-circuits, no network, still logged
  await client1.getFlakyResource(); // fails once, retry succeeds
  console.log(`\nReal network calls made: ${base1.networkCalls} (expected 3: 1 miss + 0 for hit + 2 flaky attempts)\n`);

  console.log("=== QUESTIONABLE stack: Cache -> Logging -> RateLimit -> Retry -> Fetch ===\n");
  const base2 = new FetchHttpClient();
  const client2 = new UserApiClient(buildQuestionableClient(base2, console, fastSleep));

  await client2.getUser("99"); // MISS -> logged
  await client2.getUser("99"); // HIT  -> NOT logged (logging is inside the cache!)
  console.log(`\nNotice: the second getUser produced NO log line — the cache hit never reached the logger.`);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((error) => {
    console.error(error);
    process.exit(1);
  });
}
