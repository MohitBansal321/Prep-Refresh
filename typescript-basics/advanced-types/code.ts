/**
 * TYPE GUARDS, NARROWING & DISCRIMINATED UNIONS — Production-style TypeScript example
 * ------------------------------------------------------------------------------------
 * Scenario: We are building geometry utilities for a small CAD-like tool. Shapes can
 * come from several places that do not agree on their exact representation:
 *   - Our own modern code uses a DISCRIMINATED UNION `Shape` (Circle | Square | Triangle),
 *     each tagged with a `kind` literal field.
 *   - An old, still-running part of the codebase hands us `LegacyCircle` CLASS INSTANCES
 *     instead of plain tagged objects.
 *   - Untrusted JSON arriving from a file/network has type `unknown` and might not even
 *     be a valid shape at all.
 *
 * To handle all of this safely we use TypeScript's narrowing tools:
 *   - `typeof`      -> distinguish primitives (string vs number)
 *   - `instanceof`  -> distinguish class instances
 *   - `in`          -> distinguish object shapes by which property exists
 *   - custom type predicates (`x is Foo`) -> reusable, more complex validation
 *   - discriminated unions + `switch`      -> narrow a whole family of related types
 *     at once, with a `never`-typed default case that catches missing cases at
 *     COMPILE time instead of at runtime.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. THE DOMAIN TYPES — a discriminated union, a legacy class, and raw JSON.
//    Notice: every member of Shape shares a literal `kind` field. That shared,
//    LITERAL-typed field is what makes it a "discriminated" (a.k.a. "tagged") union.
// =============================================================================

export interface Circle {
  kind: "circle";
  radius: number;
}

export interface Square {
  kind: "square";
  side: number;
}

export interface Triangle {
  kind: "triangle";
  base: number;
  height: number;
}

/** The discriminated union: a value is exactly one of these three shapes at a time. */
export type Shape = Circle | Square | Triangle;

/**
 * Legacy code (a part of the system we deliberately are not rewriting) still
 * represents circles as a CLASS instead of a tagged object. It has no `kind`
 * field at all — instanceof, not typeof or `in`, is what identifies it.
 */
export class LegacyCircle {
  constructor(public radius: number) {}
}

// =============================================================================
// 2. TYPEOF GUARD — narrow between primitive types.
//    Real input often arrives as either a string (e.g. from a form field or a
//    URL query param) or already as a number (from code that built it directly).
// =============================================================================

/**
 * Accepts a dimension that may have arrived as text (e.g. "12.5" from an
 * <input>) or already as a number, and returns a clean, validated number.
 */
export function normalizeDimension(value: string | number): number {
  if (typeof value === "string") {
    // Inside this branch, TypeScript has narrowed `value` down to `string`.
    const parsed = Number.parseFloat(value);
    if (Number.isNaN(parsed)) {
      throw new Error(`"${value}" is not a valid dimension`);
    }
    return parsed;
  }

  // No `typeof value === "number"` check needed — by elimination, this is the
  // only branch left, so TypeScript narrows `value` to `number` here too.
  return value;
}

// =============================================================================
// 3. INSTANCEOF GUARD — narrow between class instances and plain objects.
//    `instanceof` walks the prototype chain at runtime, so it only works for
//    classes — plain object literals and interfaces leave no trace at runtime.
// =============================================================================

/**
 * The old part of the system still calls us with `LegacyCircle` instances.
 * We convert those into a modern, tagged `Circle` before doing anything else.
 */
export function toModernCircle(input: Circle | LegacyCircle): Circle {
  if (input instanceof LegacyCircle) {
    // Inside this branch, TypeScript knows `input` is `LegacyCircle`.
    return { kind: "circle", radius: input.radius };
  }

  // By elimination, `input` must be `Circle` here — already tagged.
  return input;
}

// =============================================================================
// 4. IN OPERATOR GUARD — narrow by checking whether a property exists.
//    Useful when the objects in a union do NOT share a discriminant field —
//    for example, raw JSON that predates our `kind` tag convention.
// =============================================================================

/** What raw, untagged shape data looked like before we added the `kind` field. */
type RawCircle = { radius: number };
type RawSquare = { side: number };
type RawShapeData = RawCircle | RawSquare;

/** Adds the missing `kind` tag by checking which property is present. */
export function tagRawShape(raw: RawShapeData): Shape {
  if ("radius" in raw) {
    // Inside this branch, TypeScript knows `raw` is `RawCircle`.
    return { kind: "circle", radius: raw.radius };
  }

  // By elimination, `raw` must be `RawSquare` here.
  return { kind: "square", side: raw.side };
}

// =============================================================================
// 5. CUSTOM TYPE PREDICATE — `x is Foo` functions for reusable/complex checks.
//    Necessary once a check is more involved than a single typeof/instanceof/in,
//    or when the same validation logic needs to be reused in many places.
// =============================================================================

function isFiniteNumber(x: unknown): x is number {
  return typeof x === "number" && Number.isFinite(x);
}

/**
 * Validates that an untrusted `unknown` value (e.g. `JSON.parse()` output) is
 * actually a well-formed `Shape`. The `x is Shape` return type is a TYPE
 * PREDICATE: it tells the compiler "if this function returns true, treat `x`
 * as `Shape` from this point on," everywhere the function is used as a check.
 */
export function isShape(x: unknown): x is Shape {
  if (typeof x !== "object" || x === null) {
    return false;
  }

  const candidate = x as Record<string, unknown>;

  switch (candidate.kind) {
    case "circle":
      return isFiniteNumber(candidate.radius);
    case "square":
      return isFiniteNumber(candidate.side);
    case "triangle":
      return isFiniteNumber(candidate.base) && isFiniteNumber(candidate.height);
    default:
      return false;
  }
}

// =============================================================================
// 6. DISCRIMINATED UNION + SWITCH + EXHAUSTIVENESS CHECK — the main payoff.
//    Switching on the shared `kind` field narrows `shape` to the exact member
//    type inside each `case`. The `default` case assigns to a `never`-typed
//    variable — if a new Shape member is ever added without a matching `case`,
//    this line fails to COMPILE, long before it could fail at runtime.
// =============================================================================

export function area(shape: Shape): number {
  switch (shape.kind) {
    case "circle":
      // Inside this case, TypeScript knows `shape` is `Circle` — `.radius` is safe.
      return Math.PI * shape.radius ** 2;

    case "square":
      // Inside this case, TypeScript knows `shape` is `Square` — `.side` is safe.
      return shape.side ** 2;

    case "triangle":
      // Inside this case, TypeScript knows `shape` is `Triangle`.
      return 0.5 * shape.base * shape.height;

    default: {
      // Exhaustiveness check: if every case above is handled, `shape` has type
      // `never` here. Add a `Rectangle` to `Shape` tomorrow and forget to add
      // a matching `case`, and `shape` is no longer `never` — this line
      // becomes a COMPILE ERROR, not a silent runtime bug.
      const _exhaustive: never = shape;
      throw new Error(`Unhandled shape kind: ${JSON.stringify(_exhaustive)}`);
    }
  }
}

// =============================================================================
// 7. DEMO — exercise every guard against the same Shape domain.
// =============================================================================

function main(): void {
  // --- typeof guard ---
  console.log("normalizeDimension('12.5') =", normalizeDimension("12.5"));
  console.log("normalizeDimension(7)      =", normalizeDimension(7));

  // --- instanceof guard ---
  const legacy = new LegacyCircle(3);
  const fromLegacy = toModernCircle(legacy);
  console.log("toModernCircle(LegacyCircle(3)) =", fromLegacy);

  // --- in operator guard ---
  const rawSquare: RawShapeData = { side: 4 };
  console.log("tagRawShape({ side: 4 }) =", tagRawShape(rawSquare));

  // --- custom type predicate ---
  const untrusted: unknown = JSON.parse('{"kind":"triangle","base":6,"height":4}');
  if (isShape(untrusted)) {
    // `untrusted` is narrowed to `Shape` here, purely because isShape() said so.
    console.log("Untrusted JSON is a valid Shape. Area =", area(untrusted).toFixed(2));
  } else {
    console.log("Untrusted JSON was NOT a valid Shape.");
  }

  const garbage: unknown = { kind: "hexagon", sides: 6 };
  console.log("isShape(garbage) =", isShape(garbage));

  // --- discriminated union + switch + exhaustiveness ---
  const shapes: Shape[] = [
    { kind: "circle", radius: 2 },
    { kind: "square", side: 3 },
    { kind: "triangle", base: 6, height: 4 },
  ];

  for (const shape of shapes) {
    console.log(`area(${shape.kind}) =`, area(shape).toFixed(2));
  }
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
