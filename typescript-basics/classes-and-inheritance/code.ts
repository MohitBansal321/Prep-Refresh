/**
 * CLASSES AND INHERITANCE — Production-style TypeScript example
 * ---------------------------------------------------------------
 * Scenario: We model shapes. Every shape has an id, a name, and a way to
 * describe itself — but each shape computes its area completely differently.
 *
 * We solve this with a base class + subclasses:
 *   - Shape      -> base class (shared id/name/describe machinery)
 *   - Circle     -> subclass, extends Shape, FULLY overrides describe()
 *   - Rectangle  -> subclass, extends Shape, does NOT override describe()
 *
 * This single file demonstrates:
 *   1. A base class with a constructor and a method meant to be overridden.
 *   2. `extends` + `super(...)` in a subclass constructor.
 *   3. Overriding a method and calling `super.method()` inside the override.
 *   4. A `static` member accessed via the class, not an instance.
 *   5. A `readonly` field.
 *
 * Run with: ts-node code.ts   (or compile with tsc)
 */

// =============================================================================
// 1. BASE CLASS — Shape. Holds everything common to every shape.
//    `protected name` is visible here AND in subclasses, but not outside.
//    `readonly id` is assigned once, from a `static` counter shared by ALL
//    shapes (Circle, Rectangle, or any future subclass).
// =============================================================================

class Shape {
  // static: ONE copy shared by the class itself, not per instance.
  // Accessed as Shape.instanceCount, never as someShape.instanceCount.
  private static instanceCount = 0;

  // readonly: may be assigned only here, inside the constructor.
  // Any later attempt to reassign `this.id` anywhere else is a compile error.
  readonly id: number;

  constructor(protected name: string) {
    // `protected name` above is shorthand for declaring AND assigning a
    // `protected` field from a constructor parameter in one step.
    this.id = ++Shape.instanceCount;
  }

  /**
   * Meant to be overridden by every subclass — each shape computes area
   * differently. The base implementation is just a safe default.
   */
  area(): number {
    return 0;
  }

  /**
   * Shared description logic. Notice it calls `this.area()`, NOT a
   * hardcoded formula — thanks to dynamic dispatch, `this.area()` always
   * resolves to whichever subclass is actually running, even though this
   * method itself is written once, here, in the base class.
   */
  describe(): string {
    return `${this.name} (#${this.id}) has area ${this.area().toFixed(2)}`;
  }

  /** Inherited as-is by every subclass — nobody needs to override this. */
  getId(): number {
    return this.id;
  }

  /**
   * static method: called as `Shape.getInstanceCount()`. It has no `this`
   * bound to a specific shape — it reports state that belongs to the
   * class/type as a whole.
   */
  static getInstanceCount(): number {
    return Shape.instanceCount;
  }
}

// =============================================================================
// 2. SUBCLASS — Circle. `extends Shape`, calls `super(...)`, and overrides
//    BOTH area() (full replace) and describe() (extend via super.describe()).
// =============================================================================

class Circle extends Shape {
  readonly radius: number;

  constructor(radius: number) {
    // `super(...)` MUST be the first statement here. Until it returns, the
    // inherited part of the object (this.name, this.id) does not exist yet,
    // so TypeScript refuses to let you touch `this` before this line —
    // that is the "must call super before accessing this" error.
    super("Circle");

    // Only now, after super() has run, is it safe to use `this`.
    this.radius = radius;
  }

  // Full replace: Circle's area formula has nothing in common with Shape's
  // default, so there is nothing to extend — just override completely.
  override area(): number {
    return Math.PI * this.radius ** 2;
  }

  // Extend, not replace: reuse Shape.describe()'s id/name/area formatting
  // via super.describe(), then append circle-specific detail.
  override describe(): string {
    return `${super.describe()} [radius=${this.radius}]`;
  }
}

// =============================================================================
// 3. SUBCLASS — Rectangle. `extends Shape`, calls `super(...)`, but overrides
//    ONLY area(). It does NOT override describe() — proving a subclass does
//    not have to override every inherited method.
// =============================================================================

class Rectangle extends Shape {
  readonly width: number;
  readonly height: number;

  constructor(width: number, height: number) {
    super("Rectangle"); // must run before this.width / this.height are set
    this.width = width;
    this.height = height;
  }

  override area(): number {
    return this.width * this.height;
  }

  // No describe() override here on purpose. Rectangle simply inherits
  // Shape.describe() unchanged. Because Shape.describe() calls `this.area()`
  // (not Shape.area()), it still correctly reports Rectangle's own area —
  // that is dynamic dispatch / polymorphism at work.
}

// =============================================================================
// 4. STATIC VS INSTANCE — a quick, explicit demonstration.
// =============================================================================

function demonstrateStaticVsInstance(circle: Circle): void {
  // Correct: static members are accessed through the CLASS.
  console.log(`[static] Shape.getInstanceCount() = ${Shape.getInstanceCount()}`);

  // Instance members are accessed through an INSTANCE.
  console.log(`[instance] circle.getId() = ${circle.getId()}`);

  // The next line would NOT compile if uncommented — static members do not
  // exist on instances:
  //
  //   circle.getInstanceCount(); // Error: Property 'getInstanceCount' does
  //                              // not exist on type 'Circle'.
}

// =============================================================================
// 5. DEMO — construct shapes, use them polymorphically, inspect static state.
// =============================================================================

function main(): void {
  const circle = new Circle(5);
  const rectangle = new Rectangle(4, 6);

  // Polymorphism: this array is typed as Shape[], but each call to
  // describe() runs the correct subclass's behavior automatically.
  const shapes: Shape[] = [circle, rectangle];

  for (const shape of shapes) {
    console.log(shape.describe());
  }
  // Expected output:
  //   Circle (#1) has area 78.54 [radius=5]
  //   Rectangle (#2) has area 24.00      <- inherited describe(), still correct

  demonstrateStaticVsInstance(circle);
  console.log(`Total shapes ever created: ${Shape.getInstanceCount()}`);

  // readonly enforcement — this would NOT compile if uncommented:
  //
  //   circle.radius = 10; // Error: Cannot assign to 'radius' because it is
  //                       // a read-only property.
}

// Execute only when run directly (not when imported by tests).
if (require.main === module) {
  main();
}
