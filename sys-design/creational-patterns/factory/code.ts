/**
 * FACTORY METHOD PATTERN — Production-style TypeScript example
 * ------------------------------------------------------------
 * Scenario: Our backend must send notifications through several channels
 * (Email, SMS, Push). Each channel has its own concrete implementation and
 * its own configuration, but the rest of the application should NOT care
 * which one it is talking to. Business code decides "send this message to
 * this user via this channel" and nothing more.
 *
 * We show, in ONE file, the three things engineers constantly confuse:
 *
 *   (A) SIMPLE FACTORY   — one place (a function/class with a switch or map)
 *                          that decides which concrete product to build.
 *                          NOT a true GoF pattern, but the most common form.
 *
 *   (B) FACTORY METHOD   — the real GoF pattern. A Creator class declares a
 *                          factory method; subclasses (ConcreteCreators)
 *                          override it to decide which product to instantiate.
 *
 *   (C) REGISTRY FACTORY — the open/closed upgrade of the simple factory:
 *                          a map of creator functions, so adding a product
 *                          means registering a new entry, not editing a switch.
 *
 * Across all three, the CLIENT depends only on the Product interface
 * (NotificationSender), never on a concrete class and never on `new Xxx()`.
 *
 * Run with: ts-node code.ts   (or compile with tsc then `node code.js`)
 */

// =============================================================================
// 1. PRODUCT INTERFACE — the contract every notification channel must honor.
//    The client is written against THIS type and nothing else.
// =============================================================================

export interface SendResult {
  channel: string;
  providerMessageId: string;
  deliveredAt: Date;
}

/** Our own domain error. Channel-specific failures are translated into this. */
export class NotificationError extends Error {
  constructor(
    message: string,
    public readonly channel: string,
    public readonly cause?: unknown,
  ) {
    super(message);
    this.name = "NotificationError";
  }
}

/**
 * The Product. Every channel — no matter how differently it works internally —
 * is exposed to the rest of the app through THIS interface.
 */
export interface NotificationSender {
  readonly channel: string;
  send(to: string, subject: string, body: string): Promise<SendResult>;
}

// =============================================================================
// 2. CONCRETE PRODUCTS — the real implementations. Each has its own
//    dependencies and quirks, hidden behind the shared interface.
//    (Network/vendor calls are simulated to keep the file runnable.)
// =============================================================================

export interface EmailConfig {
  smtpHost: string;
  fromAddress: string;
}

export class EmailSender implements NotificationSender {
  readonly channel = "email";

  // Dependencies injected via constructor -> testable, configurable.
  constructor(private readonly config: EmailConfig) {}

  async send(to: string, subject: string, body: string): Promise<SendResult> {
    if (!to.includes("@")) {
      throw new NotificationError(`Invalid email address: ${to}`, this.channel);
    }
    // Pretend to open an SMTP connection and send the message.
    console.log(
      `[EmailSender] ${this.config.smtpHost}: ${this.config.fromAddress} -> ${to} ` +
        `| ${subject} | ${body.length} chars`,
    );
    return {
      channel: this.channel,
      providerMessageId: `email_${Date.now()}`,
      deliveredAt: new Date(),
    };
  }
}

export interface SmsConfig {
  twilioAccountSid: string;
  senderNumber: string;
}

export class SmsSender implements NotificationSender {
  readonly channel = "sms";

  constructor(private readonly config: SmsConfig) {}

  async send(to: string, _subject: string, body: string): Promise<SendResult> {
    // SMS has no subject; the interface stays uniform, this channel just
    // ignores the field it does not need.
    if (body.length > 1600) {
      throw new NotificationError("SMS body exceeds 1600 chars", this.channel);
    }
    // Pretend to call the Twilio REST API using this.config.
    console.log(
      `[SmsSender] ${this.config.twilioAccountSid}: ${this.config.senderNumber} -> ${to}`,
    );
    return {
      channel: this.channel,
      providerMessageId: `sms_${Date.now()}`,
      deliveredAt: new Date(),
    };
  }
}

export interface PushConfig {
  fcmServerKey: string;
}

export class PushSender implements NotificationSender {
  readonly channel = "push";

  constructor(private readonly config: PushConfig) {}

  async send(to: string, subject: string, body: string): Promise<SendResult> {
    // `to` here is a device token. Pretend to POST to FCM using the server key.
    console.log(
      `[PushSender] key=${this.config.fcmServerKey.slice(0, 4)}*** -> ${to} ` +
        `| ${subject}: ${body}`,
    );
    return {
      channel: this.channel,
      providerMessageId: `push_${Date.now()}`,
      deliveredAt: new Date(),
    };
  }
}

// =============================================================================
// 3. (A) SIMPLE FACTORY — the most common real-world form.
//     One function centralizes the `new` calls and the channel decision.
//     The client asks for a channel by name; it never sees a concrete class.
//
//     Downside: every new channel forces an edit to this switch (it is NOT
//     open for extension). Good enough for a small, stable set of products.
// =============================================================================

export type ChannelName = "email" | "sms" | "push";

export interface NotificationConfig {
  email: EmailConfig;
  sms: SmsConfig;
  push: PushConfig;
}

export function createSender(
  channel: ChannelName,
  config: NotificationConfig,
): NotificationSender {
  switch (channel) {
    case "email":
      return new EmailSender(config.email);
    case "sms":
      return new SmsSender(config.sms);
    case "push":
      return new PushSender(config.push);
    default: {
      // Exhaustiveness check: if a new ChannelName is added and not handled,
      // this line fails to compile — the compiler reminds you to update it.
      const _never: never = channel;
      throw new Error(`Unknown channel: ${_never}`);
    }
  }
}

// =============================================================================
// 4. (C) REGISTRY FACTORY — the open/closed upgrade of the simple factory.
//     Instead of a switch, we keep a MAP from channel name to a creator
//     function. Adding a channel = registering one entry. Existing code
//     (and this factory) does not change. This is how DI containers and
//     plugin systems build things under the hood.
// =============================================================================

type SenderCreator = (config: NotificationConfig) => NotificationSender;

export class SenderRegistry {
  private readonly creators = new Map<string, SenderCreator>();

  /** Register a way to build a sender for a channel. Open for extension. */
  register(channel: string, creator: SenderCreator): this {
    this.creators.set(channel, creator);
    return this;
  }

  /** The factory method: look up the creator and invoke it. */
  create(channel: string, config: NotificationConfig): NotificationSender {
    const creator = this.creators.get(channel);
    if (!creator) {
      throw new Error(`No sender registered for channel: ${channel}`);
    }
    return creator(config);
  }
}

/** A registry pre-populated with the built-in channels. */
export function buildDefaultRegistry(): SenderRegistry {
  return new SenderRegistry()
    .register("email", (c) => new EmailSender(c.email))
    .register("sms", (c) => new SmsSender(c.sms))
    .register("push", (c) => new PushSender(c.push));
}

// =============================================================================
// 5. (B) FACTORY METHOD (true GoF) — a Creator base class contains the
//     workflow, and defers the "which product" decision to subclasses.
//
//     Notice the Creator's real job is the WORKFLOW (validate -> build ->
//     send -> audit). Object creation is delegated to the abstract factory
//     method `createSender()`, which each ConcreteCreator overrides.
// =============================================================================

export interface NotificationRequest {
  to: string;
  subject: string;
  body: string;
}

export abstract class NotificationDispatcher {
  /**
   * THE FACTORY METHOD. Declared abstract so each subclass decides which
   * concrete NotificationSender to instantiate. The base class never says
   * `new EmailSender()` — it only says `this.createSender()`.
   */
  protected abstract createSender(): NotificationSender;

  /**
   * Template-style workflow shared by all channels. This is the "core
   * business logic that relies on the product" — the reason the Creator
   * exists at all, rather than being a bare factory function.
   */
  async dispatch(request: NotificationRequest): Promise<SendResult> {
    if (!request.body.trim()) {
      throw new NotificationError("Empty notification body", "unknown");
    }

    const sender = this.createSender(); // <- product created here, type decided by subclass

    try {
      const result = await sender.send(request.to, request.subject, request.body);
      // Shared post-send behavior (audit log, metrics) lives in the Creator.
      console.log(
        `[Dispatcher] Sent via ${result.channel}, id=${result.providerMessageId}`,
      );
      return result;
    } catch (err) {
      throw new NotificationError(
        `Dispatch failed on ${sender.channel}`,
        sender.channel,
        err,
      );
    }
  }
}

export class EmailDispatcher extends NotificationDispatcher {
  constructor(private readonly config: EmailConfig) {
    super();
  }
  protected createSender(): NotificationSender {
    return new EmailSender(this.config);
  }
}

export class SmsDispatcher extends NotificationDispatcher {
  constructor(private readonly config: SmsConfig) {
    super();
  }
  protected createSender(): NotificationSender {
    return new SmsSender(this.config);
  }
}

// =============================================================================
// 6. CLIENT — business logic. Depends only on the NotificationSender product
//    (or on the NotificationDispatcher workflow). It never mentions a
//    concrete channel class and never calls `new`.
// =============================================================================

export class UserAlertService {
  // Injected as the Product interface — never a concrete class.
  constructor(private readonly sender: NotificationSender) {}

  async alertPasswordChanged(userContact: string): Promise<void> {
    await this.sender.send(
      userContact,
      "Security alert",
      "Your password was just changed. If this was not you, contact support.",
    );
    console.log(`[UserAlertService] Alert sent over ${this.sender.channel}`);
  }
}

// =============================================================================
// 7. COMPOSITION ROOT / DEMO — the ONLY place that knows about concrete
//    classes. Everything above depends on interfaces.
// =============================================================================

const CONFIG: NotificationConfig = {
  email: { smtpHost: "smtp.example.com", fromAddress: "no-reply@example.com" },
  sms: { twilioAccountSid: "AC_demo", senderNumber: "+10000000000" },
  push: { fcmServerKey: "fcm_demo_key" },
};

async function main(): Promise<void> {
  // --- (A) Simple factory: pick a channel by name, inject into the client.
  const emailSender = createSender("email", CONFIG);
  await new UserAlertService(emailSender).alertPasswordChanged("user@example.com");

  // --- (C) Registry factory: same idea, but open for extension.
  const registry = buildDefaultRegistry();
  const smsSender = registry.create("sms", CONFIG);
  await new UserAlertService(smsSender).alertPasswordChanged("+919999999999");

  // Adding a channel later needs ONLY a register() call — no edits above:
  // registry.register("slack", (c) => new SlackSender(c /* ... */));

  // --- (B) Factory Method: subclass decides the product, Creator runs workflow.
  const dispatchers: NotificationDispatcher[] = [
    new EmailDispatcher(CONFIG.email),
    new SmsDispatcher(CONFIG.sms),
  ];
  for (const dispatcher of dispatchers) {
    await dispatcher.dispatch({
      to: dispatcher instanceof SmsDispatcher ? "+919999999999" : "user@example.com",
      subject: "Weekly digest",
      body: "Here is what happened this week.",
    });
  }
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main().catch((e) => {
    console.error(e);
    process.exit(1);
  });
}
