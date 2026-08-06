# Creational Patterns

## Table of Contents
1. [Singleton](#singleton)
2. [Factory Method](#factory-method)
3. [Abstract Factory](#abstract-factory)
4. [Builder](#builder)
5. [Prototype](#prototype)

---

## Singleton

**Concept:** Ensure a class has only one instance and provide a global access point.

**When to use:**
- Shared resource (DB connection, logger, config)
- Global state coordination
- Resource-expensive object that should be reused

**Template:**

```typescript
class Singleton {
    private static instance: Singleton;
    private constructor() {} // prevent external new

    static getInstance(): Singleton {
        if (!Singleton.instance) {
            Singleton.instance = new Singleton();
        }
        return Singleton.instance;
    }
}

const a = Singleton.getInstance();
const b = Singleton.getInstance();
console.log(a === b); // true
```

**Common mistakes:**
- Forgetting `private constructor()` — allows `new Singleton()` externally
- Not thread-safe in concurrent environments (JS is single-threaded, but be aware in Node workers)

**Practice:** Build a `Logger` class — single instance, stores logs in an array, has `log(msg)` and `logs` getter.

---

## Factory Method

**Concept:** Define an interface for creating objects, but let subclasses decide which class to instantiate.

**When to use:**
- Don't know exact types needed beforehand
- Want to extend object creation without modifying existing code
- Need to decouple creation logic from usage logic

**Template:**

```typescript
interface Product {
    operation(): string;
}

class ConcreteProductA implements Product {
    operation() { return "Product A"; }
}

abstract class Creator {
    abstract createProduct(): Product; // factory method

    doWork(): string {
        const product = this.createProduct();
        return product.operation();
    }
}

class ConcreteCreatorA extends Creator {
    createProduct() { return new ConcreteProductA(); }
}
```

**Key insight:** The Creator class contains business logic (`doWork`) that depends on the factory method. Adding a new product type means creating a new Creator subclass — no existing code changes!

**Practice:** Build a `Logistics` system with `Transport` interface (Truck, Ship, Airplane) — each `Logistics` subclass returns a different transport.

---

## Abstract Factory

**Concept:** Produce families of related objects without specifying concrete classes.

**When to use:**
- Code must work with multiple families of related products
- Want to ensure products from the same family are used together
- System should be independent of how products are created

**Template:**

```typescript
// Abstract products
interface Chair { sitOn(): void; }
interface Table { placeOn(): void; }

// Abstract factory
interface FurnitureFactory {
    createChair(): Chair;
    createTable(): Table;
}

// Concrete family
class ModernChair implements Chair { sitOn() { console.log("Modern chair"); } }
class ModernTable implements Table { placeOn() { console.log("Modern table"); } }

class ModernFactory implements FurnitureFactory {
    createChair() { return new ModernChair(); }
    createTable() { return new ModernTable(); }
}
```

**vs Factory Method:** Factory Method creates ONE product type. Abstract Factory creates FAMILIES of related products.

**Practice:** Build a `CloudFactory` (AWS vs GCP) with `Database` and `StorageBucket` product families.

---

## Builder

**Concept:** Construct complex objects step by step, avoiding telescoping constructors.

**When to use:**
- Object has many optional parameters
- Want to create different representations of the same object
- Construction involves multiple steps

**Template:**

```typescript
class Product {
    parts: string[] = [];
    listParts() { console.log(this.parts.join(", ")); }
}

interface Builder {
    buildPartA(): Builder; // return this for chaining
    buildPartB(): Builder;
    getResult(): Product;
}

class ConcreteBuilder implements Builder {
    private product = new Product();
    buildPartA() { this.product.parts.push("Part A"); return this; }
    buildPartB() { this.product.parts.push("Part B"); return this; }
    getResult() {
        const result = this.product;
        this.product = new Product(); // reset
        return result;
    }
}
```

**Optional Director:** Defines preset construction sequences:

```typescript
class Director {
    construct(builder: Builder) {
        builder.buildPartA().buildPartB();
    }
}
```

**Practice:** Build a `SQLQueryBuilder` with `.select()`, `.where()`, `.limit()` — chain methods to build a query string.

---

## Prototype

**Concept:** Clone existing objects without depending on their classes.

**When to use:**
- Creating an object is expensive (complex setup, DB calls)
- Need copies of pre-configured objects
- Want to avoid subclass explosion for minor variations

**Template:**

```typescript
interface Prototype {
    clone(): Prototype;
}

class Shape implements Prototype {
    x = 0; y = 0; color = "";

    constructor(source?: Shape) {
        if (source) {
            this.x = source.x;
            this.y = source.y;
            this.color = source.color;
        }
    }

    clone(): this {
        return new (this.constructor as any)(this);
    }
}
```

**Deep copy warning:** Arrays and nested objects need explicit copying:

```typescript
constructor(source?: Monster) {
    if (source) {
        this.skills = [...source.skills]; // shallow copy of array
        this.stats = structuredClone(source.stats); // deep copy
    }
}
```

**Practice:** Build a `Monster` class with `health`, `type`, and `skills[]` — cloning must not share the `skills` array.
