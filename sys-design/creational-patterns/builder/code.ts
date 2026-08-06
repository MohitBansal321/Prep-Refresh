/**
 * BUILDER PATTERN — Production-style TypeScript example
 * -----------------------------------------------------
 * Scenario: We are building an internal HTTP client used across many backend
 * services (NestJS microservices talking to each other, to third-party APIs,
 * and to internal gateways). Constructing an outbound HTTP request is a
 * genuinely COMPLEX operation:
 *
 *   - some fields are REQUIRED (url, method),
 *   - many are OPTIONAL (headers, query params, body, timeout, retries, auth),
 *   - some combinations are ILLEGAL (a GET request must not carry a JSON body),
 *   - and different call sites want different DEFAULTS.
 *
 * If we expressed this as a constructor we would get a "telescoping constructor":
 *     new HttpRequest(url, "GET", null, null, null, 30000, 3, undefined, ...)
 * which is unreadable and impossible to validate step by step.
 *
 * We solve it with the Builder Pattern:
 *   - HttpRequest         -> Product   (the complex, immutable object we produce)
 *   - HttpRequestBuilder  -> Builder   (interface for the build steps)
 *   - FluentHttpRequestBuilder -> ConcreteBuilder (keeps state, validates, build())
 *   - ApiRequestDirector  -> Director  (OPTIONAL — encapsulates common recipes)
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. PRODUCT — the complex object we are constructing.
//    It is IMMUTABLE: once build() hands it back, it can never be mutated.
//    Immutability matters because a request may be logged, retried, or cached,
//    and we never want a later step to silently change what was already sent.
// =============================================================================

export type HttpMethod = "GET" | "POST" | "PUT" | "PATCH" | "DELETE" | "HEAD";

/** Methods that are allowed to carry a request body. */
const METHODS_WITH_BODY: ReadonlySet<HttpMethod> = new Set(["POST", "PUT", "PATCH"]);

export class HttpRequest {
  public readonly url: string;
  public readonly method: HttpMethod;
  public readonly headers: Readonly<Record<string, string>>;
  public readonly query: Readonly<Record<string, string>>;
  public readonly body: unknown;
  public readonly timeoutMs: number;
  public readonly maxRetries: number;

  /**
   * The constructor is intentionally NOT part of the public build surface.
   * Callers never call `new HttpRequest(...)` directly — that is exactly the
   * telescoping constructor we are trying to avoid. Only the builder does,
   * after it has validated every invariant.
   */
  constructor(params: {
    url: string;
    method: HttpMethod;
    headers: Record<string, string>;
    query: Record<string, string>;
    body: unknown;
    timeoutMs: number;
    maxRetries: number;
  }) {
    this.url = params.url;
    this.method = params.method;
    // Freeze the nested objects so the returned product is deeply read-only.
    this.headers = Object.freeze({ ...params.headers });
    this.query = Object.freeze({ ...params.query });
    this.body = params.body;
    this.timeoutMs = params.timeoutMs;
    this.maxRetries = params.maxRetries;
    Object.freeze(this);
  }

  /** Turns the product into the options object a fetch-like client expects. */
  public toFetchOptions(): { url: string; init: RequestInit } {
    const qs = new URLSearchParams(this.query).toString();
    const url = qs ? `${this.url}?${qs}` : this.url;
    return {
      url,
      init: {
        method: this.method,
        headers: { ...this.headers },
        body: this.body === undefined ? undefined : JSON.stringify(this.body),
      },
    };
  }

  public describe(): string {
    return `${this.method} ${this.url} ` +
      `(headers=${Object.keys(this.headers).length}, ` +
      `query=${Object.keys(this.query).length}, ` +
      `body=${this.body === undefined ? "none" : "yes"}, ` +
      `timeout=${this.timeoutMs}ms, retries=${this.maxRetries})`;
  }
}

/** Domain error thrown when the accumulated state cannot form a valid request. */
export class RequestBuildError extends Error {
  constructor(message: string) {
    super(message);
    this.name = "RequestBuildError";
  }
}

// =============================================================================
// 2. BUILDER — the interface describing the construction steps.
//    Every step returns `this` so callers can chain (the fluent interface).
//    Programming to this interface means a Director (below) can drive ANY
//    concrete builder, not just the one we ship.
// =============================================================================

export interface HttpRequestBuilder {
  setUrl(url: string): this;
  setMethod(method: HttpMethod): this;
  addHeader(key: string, value: string): this;
  addQueryParam(key: string, value: string): this;
  setBody(body: unknown): this;
  setTimeout(ms: number): this;
  setRetries(count: number): this;
  /** Validates all accumulated state and returns the finished Product. */
  build(): HttpRequest;
  /** Clears state so the same builder instance can produce another request. */
  reset(): this;
}

// =============================================================================
// 3. CONCRETE BUILDER — accumulates mutable state, validates in build().
//    This is the only place that knows HOW to assemble an HttpRequest.
// =============================================================================

export class FluentHttpRequestBuilder implements HttpRequestBuilder {
  // Sensible defaults live here, NOT in the product. Different builders could
  // ship different defaults for different environments.
  private url?: string;
  private method?: HttpMethod;
  private headers: Record<string, string> = {};
  private query: Record<string, string> = {};
  private body: unknown = undefined;
  private timeoutMs = 30_000;
  private maxRetries = 0;

  public setUrl(url: string): this {
    this.url = url;
    return this;
  }

  public setMethod(method: HttpMethod): this {
    this.method = method;
    return this;
  }

  public addHeader(key: string, value: string): this {
    // Header names are case-insensitive; normalise to avoid duplicate keys.
    this.headers[key.toLowerCase()] = value;
    return this;
  }

  public addQueryParam(key: string, value: string): this {
    this.query[key] = value;
    return this;
  }

  public setBody(body: unknown): this {
    this.body = body;
    // Convenience: setting a JSON body implies a content-type unless overridden.
    if (this.headers["content-type"] === undefined) {
      this.headers["content-type"] = "application/json";
    }
    return this;
  }

  public setTimeout(ms: number): this {
    this.timeoutMs = ms;
    return this;
  }

  public setRetries(count: number): this {
    this.maxRetries = count;
    return this;
  }

  /**
   * build() is where the Builder pattern earns its keep. It enforces every
   * invariant in ONE place, so an invalid HttpRequest can never exist.
   * Compare this with a plain options object, which cannot self-validate.
   */
  public build(): HttpRequest {
    // --- Required-field guarantees ---
    if (!this.url) {
      throw new RequestBuildError("url is required");
    }
    if (!/^https?:\/\//.test(this.url)) {
      throw new RequestBuildError(`url must be absolute http(s): got "${this.url}"`);
    }
    if (!this.method) {
      throw new RequestBuildError("method is required");
    }

    // --- Cross-field invariants (illegal combinations) ---
    if (this.body !== undefined && !METHODS_WITH_BODY.has(this.method)) {
      throw new RequestBuildError(`a ${this.method} request must not carry a body`);
    }
    if (this.timeoutMs <= 0) {
      throw new RequestBuildError("timeout must be a positive number of ms");
    }
    if (this.maxRetries < 0) {
      throw new RequestBuildError("retries cannot be negative");
    }

    const product = new HttpRequest({
      url: this.url,
      method: this.method,
      headers: this.headers,
      query: this.query,
      body: this.body,
      timeoutMs: this.timeoutMs,
      maxRetries: this.maxRetries,
    });

    // Auto-reset so the same builder instance is safe to reuse for the next
    // request without leaking state from this one.
    this.reset();
    return product;
  }

  public reset(): this {
    this.url = undefined;
    this.method = undefined;
    this.headers = {};
    this.query = {};
    this.body = undefined;
    this.timeoutMs = 30_000;
    this.maxRetries = 0;
    return this;
  }
}

// =============================================================================
// 4. DIRECTOR (OPTIONAL) — encapsulates a common construction RECIPE so callers
//    do not repeat the same chain of steps everywhere. The Director knows the
//    ORDER and COMBINATION of steps; the Builder knows how each step works.
//
//    Note: the Director depends on the Builder INTERFACE, so it can drive any
//    concrete builder. It never calls build() itself — it returns the builder
//    so the caller can add extra steps and decide when to finish.
// =============================================================================

export class ApiRequestDirector {
  constructor(private readonly builder: HttpRequestBuilder) {}

  /** Recipe: an authenticated JSON GET to our internal API. */
  public jsonGet(baseUrl: string, path: string, bearerToken: string): HttpRequestBuilder {
    return this.builder
      .reset()
      .setUrl(`${baseUrl}${path}`)
      .setMethod("GET")
      .addHeader("Accept", "application/json")
      .addHeader("Authorization", `Bearer ${bearerToken}`)
      .setTimeout(5_000)
      .setRetries(2);
  }

  /** Recipe: an authenticated JSON POST that writes data. */
  public jsonPost(
    baseUrl: string,
    path: string,
    bearerToken: string,
    body: unknown,
  ): HttpRequestBuilder {
    return this.builder
      .reset()
      .setUrl(`${baseUrl}${path}`)
      .setMethod("POST")
      .addHeader("Accept", "application/json")
      .addHeader("Authorization", `Bearer ${bearerToken}`)
      .setBody(body)
      .setTimeout(10_000)
      .setRetries(0); // writes are not automatically retried
  }
}

// =============================================================================
// 5. DEMO — three ways to use the same building blocks.
// =============================================================================

function main(): void {
  const builder = new FluentHttpRequestBuilder();

  // (a) Direct fluent use — no Director. The caller controls every step.
  const search = builder
    .setUrl("https://api.example.com/v1/users")
    .setMethod("GET")
    .addQueryParam("role", "admin")
    .addQueryParam("active", "true")
    .setTimeout(3_000)
    .build();
  console.log("[direct]   ", search.describe());

  // (b) Director recipe, then extra caller-specific steps before build().
  const director = new ApiRequestDirector(builder);
  const createUser = director
    .jsonPost("https://api.example.com", "/v1/users", "tok_abc123", {
      email: "uday@nuvo.ai",
      role: "engineer",
    })
    .addHeader("Idempotency-Key", "req-9f2c") // extra step the recipe didn't cover
    .build();
  console.log("[director] ", createUser.describe());
  console.log("            ", JSON.stringify(createUser.toFetchOptions()));

  // (c) Validation in build() rejects an illegal combination up front.
  try {
    builder.setUrl("https://api.example.com/v1/users").setMethod("GET").setBody({ x: 1 }).build();
  } catch (err) {
    console.log("[invalid]  ", (err as Error).message); // "a GET request must not carry a body"
  }
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
