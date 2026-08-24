// ============================================================
// FUNCTIONS IN JAVASCRIPT — runnable companion to README.md
// Run: node js/functions/code.js
// ============================================================

// ------------------------------------------------------------
// 1. Declaration vs expression hoisting
// ------------------------------------------------------------
console.log("--- 1. Hoisting ---");

// A declaration is fully hoisted: callable before its definition line.
console.log(declared()); // "declaration works"

// An expression assigned with `var` is hoisted as undefined (not callable yet).
try {
  console.log(typeof varExpr); // "undefined"
  varExpr(); // would throw TypeError — commented to keep script running
} catch (e) {
  console.log("varExpr error:", e.message); // caught: calling undefined throws
}
var varExpr = function () { return "hi"; };

// An arrow assigned with `const` sits in the temporal dead zone until its line.
const constArrow = () => "later";
console.log(constArrow()); // "later"

function declared() { return "declaration works"; }

// ------------------------------------------------------------
// 2. Arrow vs regular function differences
// ------------------------------------------------------------
console.log("--- 2. Arrow vs regular ---");

function regular() {}
const arrow = () => {};

console.log("regular has prototype:", typeof regular.prototype); // "object"
console.log("arrow has prototype:", typeof arrow.prototype);     // "undefined"

try {
  new arrow();
} catch (e) {
  console.log("new arrow() ->", e.message); // "arrow is not a constructor"
}

// No own `arguments` in arrows — it resolves to the enclosing function's.
function whoHasArguments() {
  const inner = () => arguments.length; // inherits outer's arguments!
  return inner();
}
console.log("arrow sees outer arguments:", whoHasArguments(1, 2, 3)); // 3

// ------------------------------------------------------------
// 3. Default, rest, and destructured parameters
// ------------------------------------------------------------
console.log("--- 3. Parameters ---");

function connect(host = "localhost", port = host === "localhost" ? 5432 : 8080) {
  return `${host}:${port}`;
}
console.log(connect());              // "localhost:5432"
console.log(connect("db.internal")); // "db.internal:8080"
console.log(connect(null));          // "null:8080" (null does NOT trigger defaults)

function sum(first = 0, ...rest) {
  return [first, ...rest].reduce((a, b) => a + b, 0);
}
console.log(sum(1, 2, 3, 4)); // 10

function createUser({ name, role = "member", active = true } = {}) {
  return `${name} (${role}, ${active ? "active" : "inactive"})`;
}
console.log(createUser({ name: "Ada", role: "admin" })); // "Ada (admin, active)"
console.log(createUser({ name: "Ben" }));                // "Ben (member, active)"

// ------------------------------------------------------------
// 4. IIFE — private scope via immediately invoked function
// ------------------------------------------------------------
console.log("--- 4. IIFE ---");

const counterModule = (function () {
  let count = 0; // private, survives via closure
  return {
    increment() { return ++count; },
    get() { return count; },
  };
})();
counterModule.increment();
counterModule.increment();
console.log("counter value:", counterModule.get()); // 2
console.log("counter.count is:", counterModule.count); // undefined (private)

// ------------------------------------------------------------
// 5. Higher-order functions: map / filter / reduce
// ------------------------------------------------------------
console.log("--- 5. Higher-order ---");

const nums = [1, 2, 3, 4, 5, 6];
console.log("map x2:      ", nums.map(n => n * 2));                       // [2,4,6,8,10,12]
console.log("filter even: ", nums.filter(n => n % 2 === 0));              // [2,4,6]
console.log("reduce sum:  ", nums.reduce((acc, n) => acc + n, 0));         // 21

// Function returning a function:
const greeter = greeting => name => `${greeting}, ${name}!`;
console.log(greeter("Hello")("World")); // "Hello, World!"

// ------------------------------------------------------------
// 6. memoize(fn) — caching built on closures
// ------------------------------------------------------------
console.log("--- 6. Memoization ---");

function memoize(fn) {
  const cache = new Map(); // persists across calls thanks to closure
  return function (...args) {
    const key = JSON.stringify(args);
    if (!cache.has(key)) {
      cache.set(key, fn.apply(this, args));
    }
    return cache.get(key);
  };
}

let fibCalls = 0;
function slowFib(n) {
  fibCalls++;
  return n <= 1 ? n : slowFib(n - 1) + slowFib(n - 2);
}

let fastFibCalls = 0;
const fastFib = memoize(function (n) {
  fastFibCalls++;
  return n <= 1 ? n : fastFib(n - 1) + fastFib(n - 2);
});

console.log("slowFib(20):", slowFib(20), "calls:", fibCalls);   // 6765, calls: 21891
console.log("fastFib(20):", fastFib(20), "calls:", fastFibCalls); // 6765, calls: 21
fastFib(20); // cached now
console.log("repeat call count still:", fastFibCalls);           // 21 (no recompute)

// ------------------------------------------------------------
// 7. compose(f, g)(x) — small pure functions snap together
// ------------------------------------------------------------
console.log("--- 7. Composition ---");

const compose = (f, g) => x => f(g(x));            // right-to-left
const pipe = (...fns) => x => fns.reduce((v, fn) => fn(v), x); // left-to-right

const trim = s => s.trim();
const lower = s => s.toLowerCase();
const shout = s => s + "!";

console.log(compose(lower, trim)("  HeLLo  "));        // "hello"
console.log(pipe(trim, lower, shout)("  HeLLo  "));    // "hello!"
