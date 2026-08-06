/**
 * TYPES AND INFERENCE — TypeScript language feature walkthrough
 * ---------------------------------------------------------------
 * Scenario: We model a simple e-commerce Order. Every core "types and
 * inference" feature is demonstrated on the SAME domain example:
 *
 *   - OrderStatus       -> a literal-type union (a fixed set of exact strings)
 *   - Order              -> an interface (object shape, supports declaration merging)
 *   - AuditedOrder       -> an intersection type (Order & Timestamped)
 *   - OrderEvent         -> a discriminated union (for narrowing with `switch`)
 *   - ShippingPriority   -> shown three ways: enum, const enum, string-literal union
 *   - Various let/const/function examples -> TypeScript's structural inference
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. LITERAL TYPES & UNION TYPES — OrderStatus is not just `string`, it is
//    ONE OF a fixed set of exact string values. TypeScript rejects anything
//    outside the set at compile time — something plain `string` cannot do.
// =============================================================================

/** A "type alias": a name for a union of string LITERAL types. */
export type OrderStatus = "pending" | "paid" | "shipped" | "cancelled";

// A `type` alias is required here — an `interface` cannot name a union or a
// primitive; `interface OrderStatus = "pending" | ...` is not legal syntax.
// This is the sharpest, most common reason to reach for `type` over `interface`.

// =============================================================================
// 2. INTERFACE — Order (object shape). Interfaces and type aliases are nearly
//    interchangeable for plain object shapes, but we use `interface` here
//    because Order is a public, extensible domain shape (see section 7:
//    declaration merging, which ONLY interfaces support).
// =============================================================================

export interface Order {
  id: string;
  customerEmail: string;
  totalInCents: number;
  status: OrderStatus; // reuses the literal-type union from section 1
}

// The equivalent `type` alias would look identical for this shape:
//   type OrderAsType = { id: string; customerEmail: string; totalInCents: number; status: OrderStatus };
// For a plain object shape like this, `type` and `interface` behave the same
// day to day. The real differences only show up in sections 1 (unions) and 7
// (declaration merging) below.

// =============================================================================
// 3. INTERSECTION TYPES — combine two separate shapes into one with `&`.
//    Every property of BOTH sides must be present on the resulting value.
// =============================================================================

export interface Timestamped {
  createdAt: Date;
  updatedAt: Date;
}

/** An Order that also carries audit timestamps — has ALL fields of both. */
export type AuditedOrder = Order & Timestamped;

function printAuditTrail(order: AuditedOrder): void {
  console.log(
    `[Audit] Order ${order.id} (${order.status}) created ${order.createdAt.toISOString()}, ` +
      `last updated ${order.updatedAt.toISOString()}`,
  );
}

// =============================================================================
// 4. DISCRIMINATED UNIONS & NARROWING — a union of OBJECT shapes that share a
//    common literal-typed field (here, `type`). Switching/narrowing on that
//    field tells TypeScript exactly which shape you are holding.
// =============================================================================

export type OrderEvent =
  | { type: "created"; order: Order }
  | { type: "statusChanged"; orderId: string; from: OrderStatus; to: OrderStatus }
  | { type: "cancelled"; orderId: string; reason: string };

function describeEvent(event: OrderEvent): string {
  // Narrowing: inside each branch, TypeScript knows EXACTLY which member of
  // the union `event` is, and only that member's extra fields are visible.
  switch (event.type) {
    case "created":
      return `Order ${event.order.id} created for ${event.order.customerEmail}`;
    case "statusChanged":
      return `Order ${event.orderId} moved from ${event.from} to ${event.to}`;
    case "cancelled":
      return `Order ${event.orderId} cancelled: ${event.reason}`;
    default: {
      // Exhaustiveness check: if a new event variant is ever added and a case
      // is missed above, `event` will not be `never` here and this fails to compile.
      const _never: never = event;
      throw new Error(`Unhandled event: ${JSON.stringify(_never)}`);
    }
  }
}

// =============================================================================
// 5. ENUM vs CONST ENUM vs STRING-LITERAL UNION — three ways to represent
//    "one of a fixed set of options." Same concept (shipping priority),
//    three different implementations, so the tradeoffs can be compared directly.
// =============================================================================

/** Option A: a regular `enum`. Generates a real JS object at runtime. */
export enum ShippingPriorityEnum {
  Standard = "STANDARD",
  Express = "EXPRESS",
  Overnight = "OVERNIGHT",
}

/** Option B: a `const enum`. Same declaration syntax, but inlined and erased
 *  at compile time — no object is emitted; usages are replaced with the
 *  literal value. Not supported by every single-file transpiler (esbuild,
 *  Babel, SWC in isolated-modules mode) — check your build tool first. */
export const enum ShippingPriorityConstEnum {
  Standard = "STANDARD",
  Express = "EXPRESS",
  Overnight = "OVERNIGHT",
}

/** Option C: a string-literal union — the idiomatic modern TypeScript choice
 *  for most cases. Zero runtime footprint; it is erased entirely. */
export type ShippingPriority = "standard" | "express" | "overnight";

function shippingEtaDays(priority: ShippingPriority): number {
  switch (priority) {
    case "standard":
      return 5;
    case "express":
      return 2;
    case "overnight":
      return 1;
    default: {
      const _never: never = priority;
      throw new Error(`Unhandled shipping priority: ${_never}`);
    }
  }
}

// =============================================================================
// 6. TYPE INFERENCE — TypeScript figures out types WITHOUT annotations by
//    looking at the initializer, or the surrounding context. Annotate anyway
//    at certain deliberate boundaries (see the comments below).
// =============================================================================

// (a) Basic inference: `let` widens a literal initializer to its general
// type (`number`), because it can be reassigned; `const` keeps the precise
// literal type, because it never changes.
let retryCount = 0; // inferred: number
const maxRetries = 3; // inferred: 3 (the literal type, not just `number`)

// (b) Inference from an object literal: TS infers the whole shape at once.
const draftOrder = {
  id: "ORD-2001",
  customerEmail: "uday.chauhan@nuvo.ai",
  totalInCents: 149900,
  status: "pending" as OrderStatus, // widen "pending" to the OrderStatus union;
  // without `as OrderStatus`, TS would infer the narrower literal type
  // "pending" for this property — fine for a value that never changes, but
  // wrong the moment we plan to reassign `.status` to another valid status.
};

// (c) Contextual typing: TS infers a callback parameter's type from context
// (the array's element type), so `order` below is inferred as `Order` with
// NO annotation needed, because `orders` is declared as `Order[]`.
const orders: Order[] = [
  { id: "ORD-1", customerEmail: "a@example.com", totalInCents: 1000, status: "paid" },
  { id: "ORD-2", customerEmail: "b@example.com", totalInCents: 2000, status: "shipped" },
];
const totalRevenueInCents = orders.reduce((sum, order) => sum + order.totalInCents, 0);
// `sum` and `order` above are both inferred: `sum` from the accumulator's
// initial value (0 -> number), `order` contextually from `Order[]`.

// (d) Function return type inference: TS infers `string` here from the
// `return` statement. Safe to leave un-annotated for small private helpers.
function centsToDisplayString(cents: number) {
  return `$${(cents / 100).toFixed(2)}`; // inferred return type: string
}

// (e) WHY annotate anyway: function PARAMETERS are never inferred from the
// function body (TypeScript cannot read the caller's mind), so they must be
// annotated explicitly. And PUBLIC API return types should be annotated too —
// so a refactor that accidentally changes what a function returns produces a
// clear error at the function itself, not a confusing one at some distant call site.
export function applyDiscount(order: Order, percentOff: number): Order {
  return { ...order, totalInCents: Math.round(order.totalInCents * (1 - percentOff / 100)) };
}

// =============================================================================
// 7. DECLARATION MERGING — an interface can be declared MORE THAN ONCE; every
//    declaration is merged into a single shape by the compiler. A `type`
//    alias CANNOT do this — declaring `type Order = {...}` twice is a compile
//    error ("Duplicate identifier"). This merges with section 2's `Order`
//    regardless of file order, because merging happens per-scope, not by position.
// =============================================================================

export interface Order {
  /** Optional: only present once an order has shipped. Added via a SECOND
   *  declaration of `Order` — possible only because Order is an `interface`,
   *  not a `type` alias. */
  trackingNumber?: string;
}

// =============================================================================
// 8. DEMO — exercise every feature above against the same domain example.
// =============================================================================

function main(): void {
  const order: Order = {
    id: "ORD-3001",
    customerEmail: "uday.chauhan@nuvo.ai",
    totalInCents: 249900,
    status: "pending",
    trackingNumber: undefined, // legal only because of the merged declaration in section 7
  };

  console.log("--- Literal type / union check ---");
  console.log(`Order status: ${order.status}`);
  // order.status = "delivered"; // Compile error: not assignable to OrderStatus

  console.log("\n--- Intersection type ---");
  const audited: AuditedOrder = { ...order, createdAt: new Date(), updatedAt: new Date() };
  printAuditTrail(audited);

  console.log("\n--- Discriminated union narrowing ---");
  const events: OrderEvent[] = [
    { type: "created", order },
    { type: "statusChanged", orderId: order.id, from: "pending", to: "paid" },
    { type: "cancelled", orderId: order.id, reason: "Customer changed their mind" },
  ];
  for (const event of events) {
    console.log(describeEvent(event));
  }

  console.log("\n--- enum vs const enum vs string-literal union ---");
  console.log(`Enum member:        ${ShippingPriorityEnum.Express}`);
  console.log(`Const enum member:  ${ShippingPriorityConstEnum.Express} (inlined at compile time)`);
  console.log(`String literal:     express -> ETA ${shippingEtaDays("express")} day(s)`);

  console.log("\n--- Inference ---");
  console.log(`retryCount=${retryCount} (widened to number), maxRetries=${maxRetries} (literal 3)`);
  console.log("draftOrder inferred shape used directly:", draftOrder);
  console.log(`Total revenue across ${orders.length} orders: ${centsToDisplayString(totalRevenueInCents)}`);

  console.log("\n--- Explicit annotation on a public function ---");
  const discounted = applyDiscount(order, 10);
  console.log(`Discounted total: ${centsToDisplayString(discounted.totalInCents)}`);
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
