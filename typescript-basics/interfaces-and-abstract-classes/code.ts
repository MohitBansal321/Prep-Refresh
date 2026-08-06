/**
 * INTERFACES AND ABSTRACT CLASSES — Production-style TypeScript example
 * ----------------------------------------------------------------------
 * Scenario: We model a family of Notification Channels (Email, SMS). Every
 * channel must share the same logging/counting procedure, but each channel
 * decides for itself HOW a message actually leaves the building. Some
 * channels also need extra, independent capabilities (being auditable,
 * being retryable) that have nothing to do with each other.
 *
 * We solve this with interfaces + an abstract class:
 *   - Loggable / Auditable / Retryable -> interfaces (pure contracts, many
 *     can be implemented by one class, zero runtime footprint)
 *   - NotificationChannel               -> abstract class (shared, working
 *     code + one abstract method every subclass MUST supply)
 *   - EmailNotificationChannel           -> extends the abstract class AND
 *     implements two interfaces
 *   - SmsNotificationChannel             -> extends the abstract class only
 *
 * This single file demonstrates:
 *   1. A plain interface (pure shape, no implementation).
 *   2. An interface extending another interface.
 *   3. An abstract class with a constructor, fields, concrete methods, AND
 *      an abstract method with no body.
 *   4. A concrete class that both `extends` one abstract class and
 *      `implements` multiple interfaces.
 *   5. Structural typing — a class satisfying an interface's shape without
 *      ever writing `implements`.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. INTERFACES — pure contracts. No implementation is allowed here at all;
//    each one describes a shape only, and each vanishes completely once the
//    program is compiled to JavaScript.
// =============================================================================

/** Anything capable of recording a log line. A capability, not a lineage. */
interface Loggable {
  log(message: string): void;
}

/**
 * `extends Loggable` — an interface can build on another interface.
 * Anything claiming to be Auditable is automatically required to also
 * satisfy everything Loggable requires.
 */
interface Auditable extends Loggable {
  readonly auditTrailId: string;
  recordAudit(action: string): void;
}

/** A completely independent capability — nothing to do with logging or auditing. */
interface Retryable {
  readonly maxRetries: number;
  retry(recipient: string, message: string): Promise<boolean>;
}

// =============================================================================
// 2. ABSTRACT CLASS — NotificationChannel. Unlike an interface, this CAN have
//    a constructor, real fields, and fully-working ("concrete") methods.
//    It also declares ONE abstract method with no body at all — every
//    concrete subclass is compiler-forced to supply it.
// =============================================================================

abstract class NotificationChannel implements Loggable {
  // A real field, shared by every subclass instance — impossible in an interface.
  protected sentCount = 0;

  // A real constructor — impossible in an interface.
  constructor(protected readonly channelName: string) {}

  /**
   * Abstract method: NO body here. Every subclass must supply its own —
   * this is the one piece of behavior that genuinely differs per channel.
   * TypeScript will not compile a concrete subclass that fails to provide it.
   */
  abstract deliver(recipient: string, message: string): Promise<boolean>;

  /**
   * Concrete method, fully implemented ONCE, here, and shared by every
   * subclass unchanged. Notice it calls `this.deliver(...)` — thanks to
   * dynamic dispatch, that always resolves to whichever concrete subclass
   * is actually running.
   */
  async send(recipient: string, message: string): Promise<boolean> {
    this.log(`Sending via ${this.channelName} to ${recipient}`);
    const delivered = await this.deliver(recipient, message);
    if (delivered) {
      this.sentCount++;
    }
    return delivered;
  }

  /**
   * Concrete implementation of the Loggable contract. Because this lives
   * here, ANY subclass automatically satisfies Loggable (and therefore the
   * Loggable half of Auditable) without writing a single line of its own.
   */
  log(message: string): void {
    console.log(`[${this.channelName}] ${message}`);
  }

  /** Inherited as-is by every subclass — nobody needs to override this. */
  getSentCount(): number {
    return this.sentCount;
  }
}

// =============================================================================
// 3. CONCRETE CLASSES — extend the ONE abstract class, and implement AS MANY
//    interfaces as each one genuinely fulfills. No shared code conflicts
//    across interfaces, so there is no limit on how many can be stacked.
// =============================================================================

/**
 * Extends NotificationChannel (one lineage, shared code) AND implements
 * BOTH Auditable and Retryable (two independent capability contracts).
 * This is the concrete demonstration of "extends one, implements many."
 */
class EmailNotificationChannel
  extends NotificationChannel
  implements Auditable, Retryable
{
  readonly auditTrailId: string;
  readonly maxRetries = 3;

  constructor(auditTrailId: string) {
    // super(...) MUST be the first statement — until it returns, the
    // inherited part of the object (channelName, sentCount) does not exist.
    super("Email");
    this.auditTrailId = auditTrailId;
  }

  // The one method NotificationChannel forces every subclass to supply.
  async deliver(recipient: string, message: string): Promise<boolean> {
    // Pretend network call to an email provider.
    console.log(`Emailing ${recipient}: "${message}"`);
    return true;
  }

  // Fulfills the Auditable-specific member. Note: Auditable also requires
  // log(), but that is ALREADY satisfied via inheritance from
  // NotificationChannel — nothing extra needed here for that part.
  recordAudit(action: string): void {
    console.log(`[Audit ${this.auditTrailId}] ${action}`);
  }

  // Fulfills the Retryable contract — completely independent of Auditable.
  async retry(recipient: string, message: string): Promise<boolean> {
    for (let attempt = 1; attempt <= this.maxRetries; attempt++) {
      console.log(`[Retry ${attempt}/${this.maxRetries}] ${this.channelName}`);
      const ok = await this.deliver(recipient, message);
      if (ok) return true;
    }
    return false;
  }
}

/**
 * Extends NotificationChannel only. It implements NO extra interfaces —
 * proof that a concrete class never has to opt into every available
 * capability, only the ones it genuinely has.
 */
class SmsNotificationChannel extends NotificationChannel {
  constructor() {
    super("SMS");
  }

  async deliver(recipient: string, message: string): Promise<boolean> {
    console.log(`SMS to ${recipient}: "${message}"`);
    return true;
  }
}

// =============================================================================
// 4. STRUCTURAL TYPING DEMONSTRATION — TypeScript checks SHAPE, not declared
//    labels. ConsoleTracer never writes `implements Loggable`, yet it can be
//    passed anywhere a Loggable is expected, because its shape matches.
// =============================================================================

/** Notice: no `implements Loggable` anywhere on this class. */
class ConsoleTracer {
  log(message: string): void {
    console.log(`[tracer] ${message}`);
  }
}

/** Accepts anything shaped like a Loggable — it does not care about the name. */
function announce(target: Loggable, message: string): void {
  target.log(message);
}

// =============================================================================
// 5. COMPILE-TIME-ONLY CHECKS — uncomment any line below to see the errors
//    TypeScript raises. None of these are runtime checks; the emitted
//    JavaScript has no special protection once these guards are removed.
// =============================================================================

function demonstrateCompileTimeGuards(): void {
  // 1) Cannot instantiate an abstract class directly:
  //
  //   const bad = new NotificationChannel("X");
  //   // Error: Cannot create an instance of an abstract class.

  // 2) Interfaces have no runtime representation — `instanceof` cannot
  //    check against them:
  //
  //   if (someValue instanceof Loggable) { ... }
  //   // Error: 'Loggable' only refers to a type, but is being used as a
  //   // value here.

  // instanceof DOES work against the abstract class itself, because it
  // compiles down to a real JavaScript class:
  const email = new EmailNotificationChannel("trail-demo");
  console.log(`email instanceof NotificationChannel: ${email instanceof NotificationChannel}`);
}

// =============================================================================
// 6. DEMO — construct both channels, use them polymorphically, and prove the
//    structural typing point with ConsoleTracer.
// =============================================================================

async function main(): Promise<void> {
  const email = new EmailNotificationChannel("trail-42");
  const sms = new SmsNotificationChannel();

  // Polymorphism: this array is typed as NotificationChannel[], but each
  // call to send() dispatches to the correct concrete subclass's deliver().
  const channels: NotificationChannel[] = [email, sms];

  for (const channel of channels) {
    await channel.send("uday@example.com", "Your order shipped");
  }
  // Expected output includes:
  //   [Email] Sending via Email to uday@example.com
  //   Emailing uday@example.com: "Your order shipped"
  //   [SMS] Sending via SMS to uday@example.com
  //   SMS to uday@example.com: "Your order shipped"

  // Only EmailNotificationChannel fulfills Auditable and Retryable —
  // SmsNotificationChannel was never asked to, and does not have these.
  email.recordAudit("order-shipped notification sent");
  await email.retry("uday@example.com", "Retry: your order shipped");

  console.log(`Email channel sent count: ${email.getSentCount()}`);
  console.log(`SMS channel sent count: ${sms.getSentCount()}`);

  // Structural typing: ConsoleTracer satisfies Loggable purely by shape,
  // with no `implements Loggable` clause anywhere on its declaration.
  announce(new ConsoleTracer(), "structural typing works without `implements`");

  demonstrateCompileTimeGuards();
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
