# Exercise Catalog

## Table of Contents
1. [JavaScript Fundamentals](#javascript-fundamentals)
2. [Creational Design Patterns](#creational-design-patterns)
3. [Quick Quizzes](#quick-quizzes)

---

## JavaScript Fundamentals

### Easy: Primitive Type Checker
**Topic:** Primitives, typeof
**Task:** Write a function `describeType(val)` that returns a string like `"10 is a number"`. Handle the `null` gotcha correctly (should say "null", not "object").
**Expected:** `describeType(null)` → `"null is null"`, `describeType(42)` → `"42 is a number"`

### Easy: Immutability Demo
**Topic:** String immutability
**Task:** Write a function `capitalizeFirst(str)` that returns a new string with the first letter capitalized. Demonstrate that the original string is unchanged.

### Medium: Deep vs Shallow Copy
**Topic:** Object references
**Task:** Given an object `{ user: { name: "A", prefs: { theme: "dark" } } }`, create both a shallow copy and a deep copy. Modify nested `prefs.theme` in each copy and show which affects the original.

### Medium: Object Freeze Inspector
**Topic:** Object.freeze, mutation
**Task:** Write a function `deepFreeze(obj)` that recursively freezes all nested objects (since `Object.freeze` is shallow).

### Hard: Type Coercion Gauntlet
**Topic:** Type coercion, equality
**Task:** Predict the output of 10 tricky expressions like `[] == false`, `"" == 0`, `null == undefined`, `NaN === NaN`. Write tests to verify.

---

## Creational Design Patterns

### Easy: Singleton Logger
**Topic:** Singleton
**Task:** Build a `Logger` class with `.log(msg)` and `.logs` getter. Verify that two `getInstance()` calls return the same object.

### Medium: Factory — Payment Gateway
**Topic:** Factory Method
**Task:** Build `PaymentProcessor` with Credit Card and PayPal support. Each payment type has unique constructor args. Add Apple Pay without modifying existing code.

### Medium: Abstract Factory — Cloud Provider
**Topic:** Abstract Factory
**Task:** Build `CloudFactory` (AWS vs GCP) producing `Database` and `StorageBucket`. Client code works with any provider without knowing the concrete classes.

### Medium: Builder — SQL Query
**Topic:** Builder
**Task:** Build `SQLQueryBuilder` with chainable `.select()`, `.where()`, `.limit()` methods that produce a SQL string. Add a `Director` for preset queries.

### Medium: Prototype — Game Monster
**Topic:** Prototype
**Task:** Build a `Monster` class with `clone()`. Ensure `skills[]` array is deep-copied so modifying a clone's skills doesn't affect the original.

### Hard: Pattern Combo — Plugin System
**Topic:** Singleton + Factory + Builder
**Task:** Build a `PluginManager` (singleton) that uses a `PluginFactory` to create plugins, and a `PluginConfigBuilder` to configure them. Demonstrate registering and initializing 3 different plugins.

---

## Quick Quizzes

### Quiz 1: typeof null
```js
console.log(typeof null);
// What is the output?
```
**Answer:** `"object"` — This is a legacy bug in JavaScript from the first implementation.

### Quiz 2: NaN comparison
```js
console.log(NaN === NaN);
// What is the output?
```
**Answer:** `false` — NaN is the only value in JS that is not equal to itself.

### Quiz 3: Object reassignment in function
```js
function change(obj) { obj = { x: 99 }; }
const data = { x: 1 };
change(data);
console.log(data.x);
// What is the output?
```
**Answer:** `1` — Reassigning a parameter doesn't affect the original object.

### Quiz 4: const with objects
```js
const arr = [1, 2, 3];
arr.push(4);
console.log(arr);
// Does this throw an error?
```
**Answer:** No error. Output: `[1, 2, 3, 4]`. `const` prevents reassignment, not mutation.

### Quiz 5: String immutability
```js
let s = "hello";
s[0] = "H";
console.log(s);
// What is the output?
```
**Answer:** `"hello"` — Strings are immutable. Index assignment silently fails.
