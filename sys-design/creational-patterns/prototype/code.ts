/**
 * PROTOTYPE PATTERN — Production-style TypeScript example
 * -------------------------------------------------------
 * Scenario: We run a multi-tenant marketing platform. Every time a user starts
 * a new email campaign, the campaign must be pre-populated with the tenant's
 * brand defaults: sender identity, brand colours, tracking configuration,
 * localised legal footer, default content blocks, and metadata.
 *
 * Building this "from scratch" every time is EXPENSIVE and error-prone:
 *   - it needs several DB reads (brand row, sender row, default blocks),
 *   - it needs a Redis lookup for the tenant's feature flags,
 *   - and it must be assembled in exactly the right order.
 *
 * Instead we build ONE fully-configured template per tenant ONCE (the prototype),
 * register it, and then CLONE it for every new campaign. Cloning is a pure
 * in-memory copy: no DB, no network. The user then tweaks the clone.
 *
 * Participants (GoF Prototype):
 *   - Prototype<T>          -> the interface declaring clone()
 *   - EmailCampaignTemplate -> ConcretePrototype (implements clone())
 *   - PrototypeRegistry     -> optional registry/manager of named prototypes
 *   - CampaignService       -> Client (creates new objects by cloning, never by hand)
 *
 * The star of the show is clone(): it must produce a DEEP, INDEPENDENT copy that
 * PRESERVES CLASS IDENTITY (the copy must be a real EmailCampaignTemplate, not a
 * plain object), and must correctly copy Dates, Maps, arrays, and nested objects.
 *
 * Run with: ts-node code.ts   (or compile with tsc; needs lib "es2021" or later
 * plus DOM/Node types for structuredClone, which ships in Node 17+).
 */

// =============================================================================
// 0. A small, safe deep-copy helper.
//    Why not just JSON.parse(JSON.stringify(x))? Because that silently DROPS
//    functions, turns Date into a string, and turns Map/Set into {}. It also
//    THROWS on circular references. structuredClone() (Node 17+, all modern
//    browsers) handles Date, Map, Set, typed arrays, and cycles correctly.
//    Its ONE limitation: it returns a plain object — it does NOT restore your
//    class prototype. So we use it for the *data*, then rebuild the class in
//    clone(). See EmailCampaignTemplate.clone() below.
// =============================================================================

/** Deep-copies plain data (Dates, Maps, Sets, arrays, cycles) — but loses class identity. */
function deepCopyData<T>(value: T): T {
  // structuredClone is the modern, correct deep-copy primitive.
  return structuredClone(value);
}

// =============================================================================
// 1. PROTOTYPE INTERFACE — the contract. Every cloneable object declares clone().
//    Returning `this`-typed values lets subclasses stay strongly typed.
// =============================================================================

export interface Prototype<T> {
  /** Produce a deep, independent copy of this object, preserving its concrete type. */
  clone(): T;
}

// =============================================================================
// 2. DOMAIN VALUE TYPES — the nested data that makes deep-vs-shallow copy matter.
//    Notice: nested object (sender), array of objects (blocks), a Map (metadata),
//    and a Date (updatedAt). Naive copies mishandle every one of these.
// =============================================================================

export interface Sender {
  name: string;
  email: string;
}

export interface ContentBlock {
  type: "text" | "image" | "button";
  content: string;
}

// =============================================================================
// 3. CONCRETE PROTOTYPE — a fully-configured template that knows how to clone
//    itself correctly. This is the heart of the pattern.
// =============================================================================

export class EmailCampaignTemplate implements Prototype<EmailCampaignTemplate> {
  public name: string;
  public subject: string;
  public sender: Sender; // nested object  -> must be deep-copied
  public tags: string[]; // array          -> must be deep-copied
  public blocks: ContentBlock[]; // array of objects -> must be deep-copied
  public metadata: Map<string, string>; // Map -> JSON.stringify would destroy this
  public updatedAt: Date; // Date -> JSON.stringify would turn this into a string

  constructor(init: {
    name: string;
    subject: string;
    sender: Sender;
    tags?: string[];
    blocks?: ContentBlock[];
    metadata?: Map<string, string>;
    updatedAt?: Date;
  }) {
    this.name = init.name;
    this.subject = init.subject;
    this.sender = init.sender;
    this.tags = init.tags ?? [];
    this.blocks = init.blocks ?? [];
    this.metadata = init.metadata ?? new Map();
    this.updatedAt = init.updatedAt ?? new Date();
  }

  /**
   * THE clone() METHOD.
   *
   * Two responsibilities:
   *   1. DEEP-COPY all mutable nested state so the clone shares NOTHING with the
   *      original (no aliased arrays, objects, Maps, or Dates).
   *   2. PRESERVE CLASS IDENTITY: return a real `new EmailCampaignTemplate(...)`,
   *      so `clone() instanceof EmailCampaignTemplate` is true and its methods
   *      (including clone() itself) still work.
   *
   * We deep-copy the *data* with structuredClone (handles Map/Date/cycles), then
   * feed it back through the constructor to restore the prototype chain.
   */
  clone(): EmailCampaignTemplate {
    return new EmailCampaignTemplate({
      name: this.name,
      subject: this.subject,
      // Primitives copy by value; only the reference types below need care.
      sender: deepCopyData(this.sender), // new object, not the same reference
      tags: deepCopyData(this.tags), // new array
      blocks: deepCopyData(this.blocks), // new array of new objects
      metadata: deepCopyData(this.metadata), // new Map with copied entries
      updatedAt: deepCopyData(this.updatedAt), // new Date instance
    });
  }

  /** Small helper so demos read nicely. */
  describe(): string {
    const tagList = this.tags.join(", ") || "none";
    return `"${this.name}" | subject="${this.subject}" | from=${this.sender.name} | tags=[${tagList}] | blocks=${this.blocks.length}`;
  }
}

// =============================================================================
// 4. PROTOTYPE REGISTRY — an optional but very common companion.
//    It stores fully-built prototypes under a key and hands out CLONES on demand.
//    The client asks the registry for "a fresh copy of the X template" and never
//    has to know how X was originally constructed.
// =============================================================================

export class PrototypeRegistry<T extends Prototype<T>> {
  private readonly prototypes = new Map<string, T>();

  /** Register a fully-configured prototype under a key. Built once, at startup. */
  register(key: string, prototype: T): void {
    this.prototypes.set(key, prototype);
  }

  /** Return a fresh, independent CLONE — never the stored prototype itself. */
  create(key: string): T {
    const prototype = this.prototypes.get(key);
    if (!prototype) {
      throw new Error(`No prototype registered under key "${key}"`);
    }
    // Cloning here is what protects the stored prototype from mutation.
    return prototype.clone();
  }

  keys(): string[] {
    return [...this.prototypes.keys()];
  }
}

// =============================================================================
// 5. CLIENT — business logic. It creates new campaigns by CLONING a registered
//    prototype, then applying the user's per-campaign tweaks. It never rebuilds
//    the brand defaults by hand, and it never touches the DB on this hot path.
// =============================================================================

export class CampaignService {
  constructor(private readonly registry: PrototypeRegistry<EmailCampaignTemplate>) {}

  /**
   * Start a new campaign for a tenant from that tenant's default template.
   * @param tenantKey  which registered prototype to clone
   * @param overrides  the small, per-campaign differences from the template
   */
  startCampaign(
    tenantKey: string,
    overrides: { name: string; subject?: string; extraTags?: string[] },
  ): EmailCampaignTemplate {
    // 1. Clone the pre-built, expensive-to-assemble prototype (pure memory copy).
    const campaign = this.registry.create(tenantKey);

    // 2. Apply the small differences. Mutating the clone is SAFE — it shares
    //    nothing with the stored prototype, so other campaigns are unaffected.
    campaign.name = overrides.name;
    if (overrides.subject) {
      campaign.subject = overrides.subject;
    }
    if (overrides.extraTags) {
      campaign.tags.push(...overrides.extraTags);
    }
    campaign.updatedAt = new Date();

    return campaign;
  }
}

// =============================================================================
// 6. COMPOSITION ROOT / DEMO — build the prototype ONCE (imagine this assembled
//    from DB + Redis), register it, then cheaply clone it many times.
// =============================================================================

function buildRegistry(): PrototypeRegistry<EmailCampaignTemplate> {
  const registry = new PrototypeRegistry<EmailCampaignTemplate>();

  // Pretend the following was assembled from several DB rows + a Redis lookup.
  // We pay that cost ONCE, here, at startup — not on every campaign creation.
  const acmeDefault = new EmailCampaignTemplate({
    name: "Acme Default Template",
    subject: "News from Acme",
    sender: { name: "Acme Marketing", email: "hello@acme.example" },
    tags: ["brand:acme", "type:newsletter"],
    blocks: [
      { type: "image", content: "https://cdn.acme.example/logo.png" },
      { type: "text", content: "Hello from Acme!" },
      { type: "button", content: "Shop now" },
    ],
    metadata: new Map([
      ["brandColor", "#e4002b"],
      ["locale", "en-US"],
    ]),
  });

  registry.register("acme", acmeDefault);
  return registry;
}

// -----------------------------------------------------------------------------
// A deliberate contrast: how naive copies BREAK. Kept in the demo so the failure
// modes are visible and memorable — this is the #1 source of Prototype bugs.
// -----------------------------------------------------------------------------
function demonstrateShallowVsDeep(prototype: EmailCampaignTemplate): void {
  console.log("\n--- Shallow copy (Object.assign / spread): DANGEROUS ---");
  const shallow = Object.assign(new EmailCampaignTemplate(prototype), prototype);
  // The clone's `tags` is the SAME array reference as the prototype's.
  shallow.tags.push("MUTATED-BY-SHALLOW-CLONE");
  console.log("prototype.tags after mutating shallow copy:", prototype.tags);
  // ^ prototype was corrupted! Its tags now contain "MUTATED-BY-SHALLOW-CLONE".
  // Undo the damage so the rest of the demo is clean.
  prototype.tags.pop();

  console.log("\n--- JSON deep copy: LOSSY ---");
  const viaJson = JSON.parse(JSON.stringify(prototype));
  console.log("metadata after JSON round-trip:", viaJson.metadata); // {} — Map is gone
  console.log("updatedAt is a Date?", viaJson.updatedAt instanceof Date); // false — it's a string
  console.log("is a real EmailCampaignTemplate?", viaJson instanceof EmailCampaignTemplate); // false

  console.log("\n--- Proper clone(): DEEP + CLASS-PRESERVING ---");
  const proper = prototype.clone();
  proper.tags.push("only-on-this-clone");
  console.log("prototype.tags untouched:", prototype.tags); // original is safe
  console.log("metadata preserved as Map?", proper.metadata instanceof Map); // true
  console.log("updatedAt preserved as Date?", proper.updatedAt instanceof Date); // true
  console.log("is a real EmailCampaignTemplate?", proper instanceof EmailCampaignTemplate); // true
  console.log("clone can clone itself again?", typeof proper.clone === "function"); // true
}

function main(): void {
  const registry = buildRegistry();
  const service = new CampaignService(registry);

  // Create two independent campaigns by cloning the SAME prototype.
  const summerSale = service.startCampaign("acme", {
    name: "Summer Sale 2026",
    subject: "50% off everything!",
    extraTags: ["campaign:summer"],
  });

  const winterSale = service.startCampaign("acme", {
    name: "Winter Sale 2026",
    subject: "Cosy deals inside",
    extraTags: ["campaign:winter"],
  });

  console.log("Campaign A:", summerSale.describe());
  console.log("Campaign B:", winterSale.describe());

  // Proof of independence: mutating one campaign never touches the other or the
  // stored prototype. Both clones diverged only by their own overrides.
  console.log(
    "\nTags are independent? A:",
    summerSale.tags,
    "| B:",
    winterSale.tags,
  );

  // Show the failure modes of naive copying vs the correct clone().
  const acmePrototype = registry.create("acme"); // a throwaway clone to poke at
  demonstrateShallowVsDeep(acmePrototype);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
