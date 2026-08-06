/**
 * ABSTRACT FACTORY PATTERN — Production-style TypeScript example
 * --------------------------------------------------------------
 * Scenario: Our backend must run on more than one cloud. In production it uses
 * AWS (S3 + SQS + RDS). In another region — or for a customer with a data-
 * residency requirement — it must run on Google Cloud (GCS + Pub/Sub + Cloud
 * SQL). These three resources form a FAMILY: they are provisioned together,
 * they share credentials/region, and they are designed to work with each other.
 *
 * The danger we are defending against: accidentally mixing families — e.g. an
 * AWS SQS queue talking to a GCS bucket. That combination has no shared auth,
 * no shared region, and will fail at runtime in a way that is painful to debug.
 *
 * The Abstract Factory guarantees that once you pick a provider, EVERY object
 * you receive belongs to the same family. You cannot mix by accident.
 *
 *   - CloudResourceFactory        -> Abstract Factory (interface, many create* methods)
 *   - AwsFactory / GcpFactory     -> Concrete Factories (one per family)
 *   - BlobStorage / MessageQueue / SqlDatabase -> Abstract Products (interfaces)
 *   - S3Storage, SqsQueue, ... etc -> Concrete Products (one set per family)
 *   - EventPipelineService        -> Client (depends ONLY on the abstractions)
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 0. SHARED DOMAIN TYPES — vendor-neutral. No AWS/GCP types leak past here.
// =============================================================================

/** Where a piece of infrastructure lives. Vendor-neutral on purpose. */
export interface CloudConfig {
  /** Logical region name; each factory maps it to its own provider's region id. */
  region: string;
  /** Opaque credential handle (token, service-account, etc.). */
  credentials: string;
}

/** Our own domain error. Vendor-specific errors are translated into this. */
export class InfrastructureError extends Error {
  constructor(
    message: string,
    public readonly provider: string,
    public readonly cause?: unknown,
  ) {
    super(message);
    this.name = "InfrastructureError";
  }
}

// =============================================================================
// 1. ABSTRACT PRODUCTS — the interfaces our application depends on.
//    Each provider must supply a concrete implementation of every one.
//    These are expressed in OUR language, never a vendor's.
// =============================================================================

/** Object storage (a bucket): put/get bytes by key. */
export interface BlobStorage {
  readonly kind: string; // for logging/observability only
  put(key: string, data: Buffer, contentType: string): Promise<{ url: string }>;
  get(key: string): Promise<Buffer>;
}

/** Asynchronous messaging: publish a message, consume the next one. */
export interface MessageQueue {
  readonly kind: string;
  publish(topic: string, message: string): Promise<{ messageId: string }>;
  consume(topic: string): Promise<string | null>;
}

/** Relational database access. */
export interface SqlDatabase {
  readonly kind: string;
  connect(): Promise<void>;
  query<T>(sql: string, params?: unknown[]): Promise<T[]>;
}

// =============================================================================
// 2. ABSTRACT FACTORY — the contract for building a WHOLE family at once.
//    Note it is composed of several factory methods, one per product type.
//    Every concrete factory returns products that are guaranteed compatible.
// =============================================================================

export interface CloudResourceFactory {
  /** Identifies which family this factory produces (for logging/asserts). */
  readonly provider: string;
  createBlobStorage(): BlobStorage;
  createMessageQueue(): MessageQueue;
  createDatabase(): SqlDatabase;
}

// =============================================================================
// 3a. CONCRETE PRODUCTS — AWS FAMILY. These simulate the AWS SDK behind our
//     own interfaces. In a real app the constructor would hold an S3Client etc.
// =============================================================================

class S3Storage implements BlobStorage {
  readonly kind = "aws.s3";
  constructor(private readonly cfg: CloudConfig) {}

  async put(key: string, _data: Buffer, _contentType: string): Promise<{ url: string }> {
    // Real code: this.s3.send(new PutObjectCommand({...}))
    return { url: `s3://prod-bucket-${this.cfg.region}/${key}` };
  }

  async get(key: string): Promise<Buffer> {
    return Buffer.from(`bytes-of:${key}`);
  }
}

class SqsQueue implements MessageQueue {
  readonly kind = "aws.sqs";
  constructor(private readonly cfg: CloudConfig) {}

  async publish(topic: string, message: string): Promise<{ messageId: string }> {
    return { messageId: `sqs-${topic}-${message.length}-${this.cfg.region}` };
  }

  async consume(_topic: string): Promise<string | null> {
    return null; // no messages in this demo
  }
}

class RdsDatabase implements SqlDatabase {
  readonly kind = "aws.rds";
  private connected = false;
  constructor(private readonly cfg: CloudConfig) {}

  async connect(): Promise<void> {
    // Real code: open a pg pool against the RDS endpoint for this.cfg.region.
    void this.cfg.region;
    this.connected = true;
  }

  async query<T>(_sql: string, _params?: unknown[]): Promise<T[]> {
    if (!this.connected) {
      throw new InfrastructureError("RDS query before connect()", "aws");
    }
    return [] as T[];
  }
}

// =============================================================================
// 3b. CONCRETE PRODUCTS — GCP FAMILY. Same interfaces, different implementation.
// =============================================================================

class GcsStorage implements BlobStorage {
  readonly kind = "gcp.gcs";
  constructor(private readonly cfg: CloudConfig) {}

  async put(key: string, _data: Buffer, _contentType: string): Promise<{ url: string }> {
    return { url: `gs://prod-bucket-${this.cfg.region}/${key}` };
  }

  async get(key: string): Promise<Buffer> {
    return Buffer.from(`bytes-of:${key}`);
  }
}

class PubSubQueue implements MessageQueue {
  readonly kind = "gcp.pubsub";
  constructor(private readonly cfg: CloudConfig) {}

  async publish(topic: string, message: string): Promise<{ messageId: string }> {
    return { messageId: `psub-${topic}-${message.length}-${this.cfg.region}` };
  }

  async consume(_topic: string): Promise<string | null> {
    return null;
  }
}

class CloudSqlDatabase implements SqlDatabase {
  readonly kind = "gcp.cloudsql";
  private connected = false;
  constructor(private readonly cfg: CloudConfig) {}

  async connect(): Promise<void> {
    void this.cfg.region;
    this.connected = true;
  }

  async query<T>(_sql: string, _params?: unknown[]): Promise<T[]> {
    if (!this.connected) {
      throw new InfrastructureError("Cloud SQL query before connect()", "gcp");
    }
    return [] as T[];
  }
}

// =============================================================================
// 4. CONCRETE FACTORIES — one per family. Each factory ONLY ever returns
//    products from its own family. This is what makes mixing impossible.
//    Notice the shared CloudConfig is threaded into every product, so the whole
//    family shares the same region + credentials automatically.
// =============================================================================

export class AwsFactory implements CloudResourceFactory {
  readonly provider = "aws";
  constructor(private readonly cfg: CloudConfig) {}

  createBlobStorage(): BlobStorage {
    return new S3Storage(this.cfg);
  }
  createMessageQueue(): MessageQueue {
    return new SqsQueue(this.cfg);
  }
  createDatabase(): SqlDatabase {
    return new RdsDatabase(this.cfg);
  }
}

export class GcpFactory implements CloudResourceFactory {
  readonly provider = "gcp";
  constructor(private readonly cfg: CloudConfig) {}

  createBlobStorage(): BlobStorage {
    return new GcsStorage(this.cfg);
  }
  createMessageQueue(): MessageQueue {
    return new PubSubQueue(this.cfg);
  }
  createDatabase(): SqlDatabase {
    return new CloudSqlDatabase(this.cfg);
  }
}

// =============================================================================
// 5. CLIENT — business logic. Depends ONLY on the abstract factory + abstract
//    products. It has no idea whether it is running on AWS or GCP.
//    Swapping clouds requires ZERO changes to this class.
// =============================================================================

export interface IncomingEvent {
  id: string;
  payload: Buffer;
}

export class EventPipelineService {
  private readonly storage: BlobStorage;
  private readonly queue: MessageQueue;
  private readonly db: SqlDatabase;

  /**
   * The factory is injected. The service builds its family ONCE, up front.
   * Because all three came from the same factory, they are guaranteed to share
   * the same provider, region and credentials — they are a matched set.
   */
  constructor(private readonly factory: CloudResourceFactory) {
    this.storage = factory.createBlobStorage();
    this.queue = factory.createMessageQueue();
    this.db = factory.createDatabase();
  }

  async process(event: IncomingEvent): Promise<void> {
    await this.db.connect();

    // 1. Archive the raw payload in object storage.
    const { url } = await this.storage.put(
      `events/${event.id}`,
      event.payload,
      "application/octet-stream",
    );

    // 2. Record a row referencing the archived object.
    await this.db.query("INSERT INTO events(id, url) VALUES ($1, $2)", [event.id, url]);

    // 3. Fan the event out to downstream consumers.
    const { messageId } = await this.queue.publish("events.ingested", event.id);

    // Same family end-to-end — storage.kind, queue.kind, db.kind all share a provider.
    console.log(
      `[${this.factory.provider}] processed ${event.id} | ` +
        `stored=${url} | db=${this.db.kind} | queue=${this.queue.kind} | msg=${messageId}`,
    );
  }
}

// =============================================================================
// 6. COMPOSITION ROOT — the ONE place a provider is chosen, driven by config.
//    Everything downstream is provider-agnostic.
// =============================================================================

export type ProviderName = "aws" | "gcp";

export function buildFactory(provider: ProviderName, cfg: CloudConfig): CloudResourceFactory {
  switch (provider) {
    case "aws":
      return new AwsFactory(cfg);
    case "gcp":
      return new GcpFactory(cfg);
    default: {
      // Exhaustiveness check: adding a new provider without handling it here
      // becomes a COMPILE error, not a runtime surprise.
      const _never: never = provider;
      throw new Error(`Unknown provider: ${_never}`);
    }
  }
}

// =============================================================================
// 7. DEMO — the client runs identically no matter which family is injected.
// =============================================================================

async function main(): Promise<void> {
  const event: IncomingEvent = { id: "evt-1001", payload: Buffer.from("hello") };

  // In reality: read from process.env.CLOUD_PROVIDER.
  const providers: ProviderName[] = ["aws", "gcp"];

  for (const provider of providers) {
    const cfg: CloudConfig = { region: "eu-1", credentials: "***" };
    const factory = buildFactory(provider, cfg); // <- only place the family is chosen
    const pipeline = new EventPipelineService(factory); // <- client code never changes
    await pipeline.process(event);
  }
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
